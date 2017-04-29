#include "utilities.h"

void initializeRules(void)
{
	constrnts.mnwrds = 1;
	constrnts.mxwrds = 22;
	constrnts.minscore = 0;
	constrnts.mnspeed = 3;
	constrnts.mxspeed = 0;
	constrnts.smooth = 1;
	constrnts.step = 150;
	constrnts.label = 1;
}

int get_random(int rng)
{
	if (rng < 1)
	{
		return 0;
	}
	return (int)(random() % rng);
}

void my_strncpy(char *dst, char *src, size_t n)
{
	(void)strncpy(dst, src, n);
	dst[n] = '\0';
}

clock_t currTime(void)
{
	struct timeval tval;
	gettimeofday(&tval, NULL);
	return ((clock_t)((tval.tv_sec * 100) + (tval.tv_usec / 10000)));
}

void getInput(int y, int x, char *bffr, int maxlen)
{
	int chr, currlength;
	currlength = 0;
	memset(bffr, 0, maxlen + 1);
	for (chr = 0; chr != 10;) 
	{
		mvaddstr(y, x + currlength, "                    ");
		move(y, x);
		if (currlength)
		{
			addstr(bffr);
		}
		refresh();
		switch (chr = getch()) 
		{
			case 10:
				break;
			case 27:
				flushinp();
				break;
			case 4:	
			case 8:	
			case KEY_BACKSPACE:
			case 127:
				if (currlength) 
				{
					currlength--;
					echochar(chr);
					bffr[currlength] = '\0';
				}
				break;
			default:
				if (currlength != maxlen && !iscntrl(chr))
					bffr[currlength++] = chr;
				break;
		}
	}
}




int compare(struct tInfo *ainf, struct tInfo *binf) 
{
	return ainf->sockfd - binf->sockfd;
}

void initList(struct list *strt) 
{
	strt->head = strt->tail = NULL;
	strt->size = 0;
}

int insertList(struct list *strt, struct tInfo *thr_info) 
{
	if(strt->size == CLIENTS) 
	{
		return -1;
	}
	if(strt->head == NULL) 
	{
		strt->head = (struct listnode *)malloc(sizeof(struct listnode));
		strt->head->threadinfo = *thr_info;
		strt->head->next = NULL;
		strt->tail = strt->head;
	}
	else 
	{
		strt->tail->next = (struct listnode *)malloc(sizeof(struct listnode));
		strt->tail->next->threadinfo = *thr_info;
		strt->tail->next->next = NULL;
		strt->tail = strt->tail->next;
	}
	strt->size++;
	return 0;
}

int deleteList(struct list *strt, struct tInfo *thr_info) 
{
	struct listnode *curr, *temp;
	if(strt->head == NULL) 
	{
		return -1;
	}
	if(compare(thr_info, &strt->head->threadinfo) == 0) 
	{
		temp = strt->head;
		strt->head = strt->head->next;
		if(strt->head == NULL) strt->tail = strt->head;
		free(temp);
		strt->size--;
		return 0;
	}
	for(curr = strt->head; curr->next != NULL; curr = curr->next) 
	{
		if(compare(thr_info, &curr->next->threadinfo) == 0) 
		{
			temp = curr->next;
			if(temp == strt->tail) 
			{
				strt->tail = curr;
			}
			curr->next = curr->next->next;
			free(temp);
			strt->size--;
			return 0;
		}
	}
	return -1;
}
