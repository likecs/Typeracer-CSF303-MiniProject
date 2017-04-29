#include "gameLogic.h"

int comp_function(const void * a, const void * b)
{
	int as = ((struct scores*)a)->points;
	int bs = ((struct scores*)b)->points;
	return ( bs - as);
}

int insertword(void)
{
	int cnt, duplicate, lineno;
	if(SERV)
	{
		int empty[22];
		size_t i;
		char *currword, *p;
		
		for (cnt = 0, i = 0; i < 22; i++)
		{
			if (position_word[i] == -2)
			{
				empty[cnt++] = i;
			}
		}
		if (!cnt || cnt < 23 - constrnts.mxwrds)
		{	
			
			return FALSE;
		}
		lineno = empty[get_random(cnt)];
		currword = wrds.word[get_random(wrds.n)];
		do 
		{
			for (duplicate = 0, i = 0; i < 22; i++)
			{
				if (!strcmp(strings_word[i], currword)) 
				{
					duplicate = 1;
					currword = wrds.word[get_random(wrds.n)];
					break;
				}
			}
		} while (duplicate);
		my_strncpy(strings_word[lineno], currword, sizeof(strings_word[lineno]) - 1);
		position_word[lineno] = -1;
		sendWords(lineno, currword);
	}
	return TRUE;
}
int filltrie(char *filename)
{
	int fd, ignore, k, l;
	char *curpos, *newpos, *p, *q;
	char **pointer;
	char bffr[60], wordpath[MAXPATHLEN];
	size_t i, j;
	struct stat sb;
	cleartrie();
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
	wrds.rawwords = (char *)malloc(sizeof(char) * (sb.st_size + 1));
	if (read(fd, wrds.rawwords, sb.st_size) != sb.st_size) 
	{
		cleartrie();
		return 2;
	}
	wrds.rawwords[sb.st_size] = '\0';
	wrds.word = (char **)malloc(sizeof(char *) * BUFFSIZE);
	wrds.max = BUFFSIZE;
	wrds.n = 0;
	curpos = wrds.rawwords;
	
	if ((curpos = strchr(curpos, '\n')) == NULL) 
	{
		cleartrie();
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
			wrds.word[wrds.n++] = curpos;
		}
		if (wrds.n >= wrds.max) 
		{
			pointer = realloc(wrds.word, wrds.max + BUFFSIZE);
			wrds.word = pointer;
			wrds.max += BUFFSIZE;
		}
		curpos = newpos;
	}
	for (i = 0; i < wrds.n; i++)
	{
		for (j = i + 1; j < wrds.n; j++)
		{
			if (!strcmp(wrds.word[i], wrds.word[j]))
			{
				wrds.word[j] = wrds.word[--wrds.n];
			}
		}
	}
	if (wrds.n < 22) 
	{
		
		cleartrie();
		return 3;
	}
	writeServerLog("Dictonary Loaded");
	return 0;
}

void deleteword(int y, int x, size_t len)
{
	move(y, x);
	while (len--)
	{
		addch(' ');
	}
}

static void checkWords(void)
{
	int i, wordcnt;
	unsigned long len;
	for (wordcnt = 0, len = 0, i = 0; i < 22; i++)
	{
		if (position_word[i] > -2) 
		{
			wordcnt++;
			len += strlen(strings_word[i]) + 1;
		}
	}
	if (wordcnt < constrnts.mnwrds)
	{
		insertword();
	}
	if (constrnts.smooth)
	{
		rate = (float)curr_stat.points / constrnts.step + constrnts.mnspeed;
	}
	else
	{
		rate = curr_stat.points / constrnts.step + constrnts.mnspeed;
	}
	if (constrnts.mxspeed && rate > constrnts.mxspeed)
	{
		rate = constrnts.mxspeed;
	}
}

static void word_animate(void)
{
	int i, len;
	for (i = 0; i < 22; i++) 
	{
		if (position_word[i] == -2)
		{
			continue;
		}
		len = (int)strlen(strings_word[i]);
		if (position_word[i] > (79 - len)) 
		{
			deleteword(i, position_word[i], len);
			position_word[i] = -2;
			continue;
		}
		if (position_word[i] > -1)
		{
			mvaddch(i, position_word[i], ' ');
		}
		mvaddstr(i, ++position_word[i], strings_word[i]);
	}
}

void cleartrie(void)
{
	if (wrds.rawwords != NULL) 
	{
		free(wrds.rawwords);
		wrds.rawwords = NULL;
	}
	if (wrds.word != NULL) 
	{
		free(wrds.word);
		wrds.word = NULL;
	}
	wrds.n = wrds.max = 0;
}

static void checkwrdinp(int *escpe, char *wrdinp, clock_t *stime, unsigned *char_position, int *wordset, int *wrongwrds)
{
	int i, prevpos, foundword;
	clock_t pausetime;
	if ((prevpos = getch()) == ERR)
	{
		return;
	}
	switch (tolower(prevpos)) 
	{
		case KEY_HOME:
		case 1: 
			*char_position = 0;
			break;
		case 27: 
			*escpe = 1;
			break;
		case 32: 
		case 10: 
			foundword = 0;
			for (i = 0; i < 22; i++) 
			{
				if (strcmp(wrdinp, strings_word[i]))
				{
					continue;
				}
				foundword = 1;
				curr_stat.numofwrds++;
				curr_stat.points += *char_position;
				deleteword(i, position_word[i], strlen(strings_word[i]));
				position_word[i] = -2;
				my_strncpy(strings_word[i], " ", sizeof(strings_word[i]) - 1);
			}
			if (!foundword) 
			{
				(*wordset)++;
				(*wrongwrds)++;
			}
			deleteword(23, 2, 19);
			if(foundword)
			{
				if(SERV)
				{
					sendFoundWords_server(wrdinp);
				}
				else if(CLIENT)
				{
					sendFoundWords_client(wrdinp);
				}
			}
			*char_position = 0;
			wrdinp[0] = '\0';
			break;
		case 11: 
			if ((size_t)*char_position < strlen(wrdinp)) 
			{
				wrdinp[*char_position] = '\0';
				deleteword(23, 2 + *char_position, 19 - *char_position);
			}
			break;
		case 21: 
			if (*char_position) 
			{
				deleteword(23, 2, *char_position);
				*char_position = 0;
				wrdinp[0] = '\0';
			}
			break;
		case 8:
		case 127:
		case KEY_BACKSPACE:
			if (*char_position) 
			{
				mvaddch(23, 1 + *char_position, ' ');
				wrdinp[--(*char_position)] = '\0';
			}
			break;
		case KEY_END:
		case 5: 
			*char_position = strlen(wrdinp);
			break;
		case KEY_LEFT:
		case 2: 
			if (*char_position)
			{
				(*char_position)--;
			}
			break;
		case KEY_RIGHT:
		case 6: 
			if (*char_position < 19 && (size_t)*char_position < strlen(wrdinp))
			{
				(*char_position)++;
			}
			break;
		default:
			if (prevpos > 255)
			{
				break;
			}
			(*wordset)++;
			if (*char_position > 18 || iscntrl(prevpos))
			{
				break;
			}
			mvaddch(23, 2 + *char_position, prevpos);
			if (*char_position == strlen(wrdinp))
			{
				wrdinp[*char_position + 1] = '\0';
			}
			wrdinp[(*char_position)++] = prevpos;
			break;
	}
	move(23, 2 + *char_position);
}

int maingameLogic()
{
	if(SERV)
	writeServerLog("Game Started!!");
	else if(CLIENT)
	writeClientLog("Game Started!!");
	int i, wordset, wordcnt, wrongwrds;
	unsigned char_position, wordlim;
	clock_t prevtime, stime;
	char wrdinp[20];
	time_t wait;
	memset(&curr_stat, 0, sizeof(curr_stat));
	curr_stat.sinit = info.seed ? info.seed : (uint32_t)time(NULL);
	srandom(curr_stat.sinit);
	escpe = char_position = wordset = 0;
	rate = 1;
	wordlim = (constrnts.mxwrds == 1) ? 0 : 1;
	screen_create();
	halfdelay(1);
	prevtime = stime = currTime();
	wrdinp[0] = '\0';

	for (i = 0; i < 22; i++) {
		strings_word[i][0] = ' ';
		strings_word[i][1] = '\0';
		position_word[i] = -2;
	}

	
	status_create(0);
	wait = time(NULL) + 1;


	while (!escpe) {

		if(time(NULL) == wait) {
			wait = time(NULL) + 1;
			timerval--;
			if(timerval%10==0)
				constrnts.mnwrds++;
		}
		if(!timerval)break;



		wordcnt = curr_stat.numofwrds;
		wrongwrds = 0;
		while (currTime() < (prevtime + (100.0 / rate))) {
			checkwrdinp(&escpe, wrdinp, &stime, &char_position,
					   &wordset, &wrongwrds);
		}
		prevtime = currTime();

		word_animate();
		checkWords();

		curr_stat.duration = currTime() - stime;
		curr_stat.game_cps = (curr_stat.points + curr_stat.numofwrds) * 100.0 / curr_stat.duration;
		curr_stat.game_tcps = (wordset + curr_stat.numofwrds) * 100.0 / curr_stat.duration;

		status_create(char_position);

		
		refresh();
	}
	if(SERV)
	writeServerLog("GAME OVER!");
	else if(CLIENT)
	writeClientLog("GAME OVER!");
	nocbreak();
	cbreak();
	memset(&final[scoreReceive], 0, sizeof(struct scores));
	final[scoreReceive].points = curr_stat.points;
	strcpy(final[scoreReceive].name ,info.name);
	scoreReceive++;
	if(CLIENT)
	{
		int sent;
		struct cmdmsg spacket;
		memset(&spacket, 0, sizeof(struct cmdmsg));
		strcpy(spacket.infoion, "SCORES");
		strcpy(spacket.plyname, info.name);
		sprintf(spacket.buff,"%d", curr_stat.points);
		if(escpe!=2)
		sent = send(sockfd, (void *)&spacket, sizeof(struct cmdmsg), 0);
		writeClientLog("Scores sent to server");
	}
	else if(SERV)
	{
		struct listnode *curr;
		int bytes, sent;
		pthread_mutex_lock(&mutex_clients);
		for(curr = info_clients.head; curr != NULL; curr = curr->next) 
		{
			struct cmdmsg spacket;
			memset(&spacket, 0, sizeof(struct cmdmsg));
			strcpy(spacket.infoion, "SCORES");
			strcpy(spacket.plyname, info.name);
			sprintf(spacket.buff,"%d", curr_stat.points);
			sent = send(curr->threadinfo.sockfd, (void *)&spacket, sizeof(struct cmdmsg), 0);
		}
		pthread_mutex_unlock(&mutex_clients);
		writeServerLog("All scores sent to all clients");
	}
	clear();
	mvaddstr(12, 30, _("GAME OVER!"));
	refresh();
	curr_stat.duration = prevtime - stime;
	if (curr_stat.game_tcps)
	{
		curr_stat.typoratio = (1 - curr_stat.game_cps / curr_stat.game_tcps) * 100;
	}
	sleep(3);

	clear();
	refresh();
	

	deleteword(12, 30, strlen(_("GAME OVER!")));
	mvaddstr(3, 20, "TypeRacer v1.0");
	mvaddstr(5, 20, _("You achieved:"));
	mvprintw(9, 20, _("points:\t\t%d"), curr_stat.points);
	mvprintw(10, 20, _("Total CPS:\t\t%2.3f"), curr_stat.game_tcps);
	mvprintw(11, 20, _("Correct CPS:\t%2.3f"), curr_stat.game_cps);
	mvprintw(12, 20, _("Typo typoratio:\t\t%2.1f%%"), curr_stat.typoratio);
	qsort(final, scoreReceive, sizeof(struct scores), comp_function);
	mvaddstr(14, 20, _("Rankings:"));
	for(int i = 0; i < scoreReceive; i++)
	{
		mvprintw(15+i, 20, _("%d. %s %d"),i+1, final[i].name, final[i].points);
	}
	
	if(SERV)
	writeServerLog("Scoreboard Displayed");
	else if(CLIENT)
	writeClientLog("Scoreboard Displayed");
	pressanykey(19, 20);

	return 0;
}

