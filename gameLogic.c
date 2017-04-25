#include "gameLogic.h"

int cmpfunc(const void * a, const void * b)
{
	int as = ((struct scores*)a)->score;
	int bs = ((struct scores*)b)->score;
	return ( bs - as);
}

int addnewword(void)
{
	int count, dup, slot;
	if(SERV)
	{
		int freeslot[22];
		size_t i;
		char *myword, *p;
		/* take a random free slot */
		for (count = 0, i = 0; i < 22; i++)
		{
			if (wordpos[i] == -2)
			{
				freeslot[count++] = i;
			}
		}
		if (!count || count < 23 - rules.maxwords)
		{	
			/* the player has enough trouble... */
			return FALSE;
		}
		slot = freeslot[get_random(count)];
		myword = words.word[get_random(words.n)];
		do 
		{
			for (dup = 0, i = 0; i < 22; i++)
			{
				if (!strcmp(wordstring[i], myword)) 
				{
					dup = 1;
					myword = words.word[get_random(words.n)];
					break;
				}
			}
		} while (dup);
		my_strncpy(wordstring[slot], myword, sizeof(wordstring[slot]) - 1);
		wordpos[slot] = -1;
		sendWords(slot, myword);
	}
	return TRUE;
}

void clearword(int y, int x, size_t length)
{
	move(y, x);
	while (length--)
	{
		addch(' ');
	}
}

static void cwords(void)
{
	int i, wc;
	unsigned long length;
	for (wc = 0, length = 0, i = 0; i < 22; i++)
	{
		if (wordpos[i] > -2) 
		{
			wc++;
			length += strlen(wordstring[i]) + 1;
		}
	}
	if (wc < rules.minwords)
	{
		addnewword();
	}
	if (rules.smooth)
	{
		rate = (float)now.score / rules.step + rules.minspeed;
	}
	else
	{
		rate = now.score / rules.step + rules.minspeed;
	}
	if (rules.maxspeed && rate > rules.maxspeed)
	{
		rate = rules.maxspeed;
	}
}

static void movewords(void)
{
	int i, length;
	for (i = 0; i < 22; i++) 
	{
		if (wordpos[i] == -2)
		{
			continue;
		}
		length = (int)strlen(wordstring[i]);
		if (wordpos[i] > (79 - length)) 
		{
			clearword(i, wordpos[i], length);
			wordpos[i] = -2;
			continue;
		}
		if (wordpos[i] > -1)
		{
			mvaddch(i, wordpos[i], ' ');
		}
		mvaddstr(i, ++wordpos[i], wordstring[i]);
	}
}

static void parseinput(int *escend, char *input, clock_t *starttime, unsigned *inputpos, int *nappaykset, int *wrngret)
{
	int i, nappi, foundword;
	clock_t pausetime;
	if ((nappi = getch()) == ERR)
	{
		return;
	}
	switch (tolower(nappi)) 
	{
		case 8:
		case 127:
		case KEY_BACKSPACE:
			if (*inputpos) 
			{
				mvaddch(23, 1 + *inputpos, ' ');
				input[--(*inputpos)] = '\0';
			}
			break;
		case 27: /* ESC */
			*escend = 1;
			break;
		case KEY_UP:
			pausetime = timenow();
			mvaddstr(23, 2, _("       PAUSED       "));
			move(23, 2);
			while (getch() == ERR)
				;
			*starttime += timenow() - pausetime;
			mvaddstr(23, 2, "                    ");
			flushinp();
		/* FALLTHROUGH */
		case 21: /* ^U */
			if (*inputpos) 
			{
				clearword(23, 2, *inputpos);
				*inputpos = 0;
				input[0] = '\0';
			}
			break;
		case 32: /* SPACE */
		case 10: /* ENTER */
			foundword = 0;
			for (i = 0; i < 22; i++) 
			{
				if (strcmp(input, wordstring[i]))
				{
					continue;
				}
				foundword = 1;
				now.wordswritten++;
				now.score += *inputpos;
				clearword(i, wordpos[i], strlen(wordstring[i]));
				wordpos[i] = -2;
				my_strncpy(wordstring[i], " ", sizeof(wordstring[i]) - 1);
			}
			if (!foundword) 
			{
				(*nappaykset)++;
				(*wrngret)++;
			}
			clearword(23, 2, 19);
			if(foundword)
			{
				if(SERV)
				{
					sendFoundWords_server(input);
				}
				else if(CLIENT)
				{
					sendFoundWords_client(input);
				}
			}
			*inputpos = 0;
			input[0] = '\0';
			break;
		case 11: /* ^K */
			if ((size_t)*inputpos < strlen(input)) 
			{
				input[*inputpos] = '\0';
				clearword(23, 2 + *inputpos, 19 - *inputpos);
			}
			break;
		case KEY_RIGHT:
		case 6: /* ^F */
			if (*inputpos < 19 && (size_t)*inputpos < strlen(input))
			{
				(*inputpos)++;
			}
			break;
		case KEY_END:
		case 5: /* ^E */
			*inputpos = strlen(input);
			break;
		case KEY_LEFT:
		case 2: /* ^B */
			if (*inputpos)
			{
				(*inputpos)--;
			}
			break;
		case KEY_HOME:
		case 1: /* ^A */
			*inputpos = 0;
			break;
		default:
			if (nappi > 255)
			{
				break;
			}
			(*nappaykset)++;
			if (*inputpos > 18 || iscntrl(nappi))
			{
				break;
			}
			mvaddch(23, 2 + *inputpos, nappi);
			if (*inputpos == strlen(input))
			{
				input[*inputpos + 1] = '\0';
			}
			input[(*inputpos)++] = nappi;
			
			for (i = 0; i < 22; i++) 
			{
				if (strcmp(input, wordstring[i]))
				{
					continue;
				}
				now.score += *inputpos;
				clearword(i, wordpos[i], strlen(wordstring[i]));
				wordpos[i] = -2;
				clearword(23, 2, 19);
				*inputpos = 0;
				input[0]= '\0';
				my_strncpy(wordstring[i], " ", sizeof(wordstring[i]) - 1);
			}
			break;
	}
	move(23, 2 + *inputpos);
}

int play(void)
{
	if(SERV)
	writeServerLog("Game Started!!");
	else if(CLIENT)
	writeClientLog("Game Started!!");
	int i, nappaykset, wordcount, wrngret;
	unsigned inputpos, wordlim;
	float min10;
	clock_t oldtimes, starttime;
	char input[20];
	time_t wait;
	memset(&now, 0, sizeof(now));
	now.sinit = opt.seed ? opt.seed : (uint32_t)time(NULL);
	srandom(now.sinit);
	min10 = 0;
	escend = inputpos = nappaykset = 0;
	rate = 1;
	wordlim = (rules.maxwords == 1) ? 0 : 1;
	drawscreen();
	halfdelay(1);
	oldtimes = starttime = timenow();
	input[0] = '\0';

	for (i = 0; i < 22; i++) {
		wordstring[i][0] = ' ';
		wordstring[i][1] = '\0';
		wordpos[i] = -2;
	}

	/* move cursor to correct position */
	drawstatus(0);
	wait = time(NULL) + 1;


	while (!escend) {

		if(time(NULL) == wait) {
			wait = time(NULL) + 1;
			timerval--;
			if(timerval%10==0)
				rules.minwords++;
		}
		if(!timerval)break;



		wordcount = now.wordswritten;
		wrngret = 0;
		while (timenow() < (oldtimes + (100.0 / rate))) {
			parseinput(&escend, input, &starttime, &inputpos,
					   &nappaykset, &wrngret);
			if (now.wordswritten - wordcount > wordlim &&
					wrngret > 10) {
				escend = 1;
				rules.label = 0;
				memset(&now, 0, sizeof(now));
				nappaykset = 0;
			}
		}
		oldtimes = timenow();

		movewords();
		cwords();

		now.duration = timenow() - starttime;
		now.speed = (now.score + now.wordswritten) * 100.0 / now.duration;
		now.totalspeed = (nappaykset + now.wordswritten) * 100.0 / now.duration;

		drawstatus(inputpos);

		/* last, we check if the opponent has new words for us... */

		//receiveWords();
		refresh();
	}
	if(SERV)
	writeServerLog("GAME OVER!");
	else if(CLIENT)
	writeClientLog("GAME OVER!");
	nocbreak();
	cbreak();
	memset(&final[scoreReceive], 0, sizeof(struct scores));
	final[scoreReceive].score = now.score;
	strcpy(final[scoreReceive].name ,opt.name);
	scoreReceive++;
	if(CLIENT)
	{
		int sent;
		struct PACKET spacket;
		memset(&spacket, 0, sizeof(struct PACKET));
		strcpy(spacket.option, "SCORES");
		strcpy(spacket.alias, opt.name);
		sprintf(spacket.buff,"%d", now.score);
		sent = send(sockfd, (void *)&spacket, sizeof(struct PACKET), 0);
		writeClientLog("Scores sent to server");
	}
	else if(SERV)
	{
		struct LLNODE *curr;
		int bytes, sent;
		pthread_mutex_lock(&clientlist_mutex);
		for(curr = client_list.head; curr != NULL; curr = curr->next) 
		{
			struct PACKET spacket;
			memset(&spacket, 0, sizeof(struct PACKET));
			strcpy(spacket.option, "SCORES");
			strcpy(spacket.alias, opt.name);
			sprintf(spacket.buff,"%d", now.score);
			sent = send(curr->threadinfo.sockfd, (void *)&spacket, sizeof(struct PACKET), 0);
		}
		pthread_mutex_unlock(&clientlist_mutex);
		writeServerLog("All scores sent to all clients");
	}
	clear();
	mvaddstr(12, 30, _("GAME OVER!"));
	refresh();
	now.duration = oldtimes - starttime;
	now.tcount = nappaykset;
	if (now.totalspeed)
	{
		now.ratio = (1 - now.speed / now.totalspeed) * 100;
	}
	sleep(3);
	while(scoreReceive != (CLIENTS + 1));

	clear();
	refresh();
	/* total speed */
	min10 = now.speed * 600;
	clearword(12, 30, strlen(_("GAME OVER!")));
	mvaddstr(3, 20, "TypeRacer v1.0");
	mvaddstr(5, 20, _("You achieved:"));
	mvprintw(9, 20, _("Score:\t\t%d"), now.score);
	mvprintw(10, 20, _("Total CPS:\t\t%2.3f"), now.totalspeed);
	mvprintw(11, 20, _("Correct CPS:\t%2.3f"), now.speed);
	mvprintw(12, 20, _("Typo ratio:\t\t%2.1f%%"), now.ratio);
	qsort(final, CLIENTS+1, sizeof(struct scores), cmpfunc);
	mvaddstr(14, 20, _("Rankings:"));
	for(int i = 0; i < CLIENTS+1; i++)
	{
		mvprintw(15+i, 20, _("%d. %s %d"),i+1, final[i].name, final[i].score);
	}
	/* these functions change color */
	if(SERV)
	writeServerLog("Scoreboard Displayed");
	else if(CLIENT)
	writeClientLog("Scoreboard Displayed");
	pressanykey(19, 20);

	return 0;
}

void freewords(void)
{
	if (words.bulk != NULL) 
	{
		free(words.bulk);
		words.bulk = NULL;
	}
	if (words.word != NULL) 
	{
		free(words.word);
		words.word = NULL;
	}
	words.n = words.max = 0;
}

int loadwords(char *filename)
{
	int fd, ignore, k, l;
	char *curpos, *newpos, *p, *q;
	char **pointer;
	char buf[60], wordpath[MAXPATHLEN];
	size_t i, j;
	struct stat sb;
	freewords();
	strcpy(wordpath,filename);
	if ((fd = open(wordpath, O_RDONLY, 0)) == -1)
	{
		return 2;
	}
	if (fstat(fd, &sb) == -1)
	{
		return 2;
	}
	if (sb.st_size >= SSIZE_MAX)
	{
		return 3;
	}
	words.bulk = (char *)malloc(sizeof(char) * (sb.st_size + 1));
	if (read(fd, words.bulk, sb.st_size) != sb.st_size) 
	{
		freewords();
		return 2;
	}
	words.bulk[sb.st_size] = '\0';
	words.word = (char **)malloc(sizeof(char *) * BUFFSIZE);
	words.max = BUFFSIZE;
	words.n = 0;
	curpos = words.bulk;
	/* ignore title of word list */
	if ((curpos = strchr(curpos, '\n')) == NULL) 
	{
		freewords();
		return 3;
	}
	while ((p = strchr(curpos, '\n')) != NULL) 
	{
		*p = '\0';
		newpos = p + 1;
		if ((p = strchr(curpos, '\r')) != NULL)
		{
			*p = '\0';
		}
		if (curpos[0] == '\0' || strlen(curpos) > 19) 
		{
			curpos = newpos;
			continue;
		}
		for (ignore = 0, p = curpos; *p != '\0'; p++)
		{
			if (!isprint(*p) || isspace(*p)) 
			{
				ignore = 1;
				break;
			}
		}
		if (!ignore)
		{
			words.word[words.n++] = curpos;
		}
		if (words.n >= words.max) 
		{
			pointer = realloc(words.word, words.max + BUFFSIZE);
			words.word = pointer;
			words.max += BUFFSIZE;
		}
		curpos = newpos;
	}
	for (i = 0; i < words.n; i++)
	{
		for (j = i + 1; j < words.n; j++)
		{
			if (!strcmp(words.word[i], words.word[j]))
			{
				words.word[j] = words.word[--words.n];
			}
		}
	}
	if (words.n < 22) 
	{
		/* not enough words */
		freewords();
		return 3;
	}
	writeServerLog("Dictonary Loaded");
	return 0;
}
