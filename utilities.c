#include "utilities.h"

void initializeRules(void)
{
	rules.label = 1;
	rules.minwords = 1;
	rules.maxwords = 22;
	rules.minscore = 0;
	rules.minspeed = 3;
	rules.maxspeed = 0;
	rules.step = 150;
	rules.smooth = 1;
	my_strncpy(rules.fname, "default", sizeof(rules.fname) - 1);
	my_strncpy(rules.name, _("default"), sizeof(rules.name) - 1);
}

int get_random(int range)
{
	if (range < 1)
	{
		return 0;
	}
	return (int)(random() % range);
}

void my_strncpy(char *dst, char *src, size_t n)
{
	(void)strncpy(dst, src, n);
	dst[n] = '\0';
}

clock_t timenow(void)
{
	struct timeval tval;
	gettimeofday(&tval, NULL);
	return ((clock_t)((tval.tv_sec * 100) + (tval.tv_usec / 10000)));
}

void getInput(int y, int x, char *buf, int maxlen)
{
	int ch, curlen;
	curlen = 0;
	memset(buf, 0, maxlen + 1);
	for (ch = 0; ch != 10;) 
	{
		mvaddstr(y, x + curlen, "                    ");
		move(y, x);
		if (curlen)
		{
			addstr(buf);
		}
		refresh();
		switch (ch = getch()) 
		{
			case KEY_BACKSPACE:
			case 4:		/* EOT */
			case 8:		/* BS */
			case 127:	/* DEL */
				if (curlen) 
				{
					echochar(ch);
					curlen--;
					buf[curlen] = '\0';
				}
				break;
			case 10:
				break;
			case 27:	/* ESC */
				flushinp();
				break;
			default:
				if (curlen != maxlen && !iscntrl(ch))
					buf[curlen++] = ch;
				break;
		}
	}
}




int compare(struct THREADINFO *a, struct THREADINFO *b) 
{
	return a->sockfd - b->sockfd;
}

void list_init(struct LLIST *root) 
{
	root->head = root->tail = NULL;
	root->size = 0;
}

int list_insert(struct LLIST *root, struct THREADINFO *thr_info) 
{
	if(root->size == CLIENTS) 
	{
		return -1;
	}
	if(root->head == NULL) 
	{
		root->head = (struct LLNODE *)malloc(sizeof(struct LLNODE));
		root->head->threadinfo = *thr_info;
		root->head->next = NULL;
		root->tail = root->head;
	}
	else 
	{
		root->tail->next = (struct LLNODE *)malloc(sizeof(struct LLNODE));
		root->tail->next->threadinfo = *thr_info;
		root->tail->next->next = NULL;
		root->tail = root->tail->next;
	}
	root->size++;
	return 0;
}

int list_delete(struct LLIST *root, struct THREADINFO *thr_info) 
{
	struct LLNODE *curr, *temp;
	if(root->head == NULL) 
	{
		return -1;
	}
	if(compare(thr_info, &root->head->threadinfo) == 0) 
	{
		temp = root->head;
		root->head = root->head->next;
		if(root->head == NULL) root->tail = root->head;
		free(temp);
		root->size--;
		return 0;
	}
	for(curr = root->head; curr->next != NULL; curr = curr->next) 
	{
		if(compare(thr_info, &curr->next->threadinfo) == 0) 
		{
			temp = curr->next;
			if(temp == root->tail) 
			{
				root->tail = curr;
			}
			curr->next = curr->next->next;
			free(temp);
			root->size--;
			return 0;
		}
	}
	return -1;
}
