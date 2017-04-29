#ifndef utilities
#define utilities

#include "structDef.h"

#endif

void initializeRules(void);
int get_random(int range);
void my_strncpy(char *dst, char *src, size_t n);
clock_t currTime(void);
void getInput(int y, int x, char *bffr, int maxlen);
int compare(struct tInfo *a, struct tInfo *b); 
void initList(struct list *ll);
int insertList(struct list *ll, struct tInfo *thr_info);
int deleteList(struct list *ll, struct tInfo *thr_info);
