#ifndef utilities
#define utilities

#include "structDef.h"

#endif

void initializeRules(void);
int get_random(int range);
void my_strncpy(char *dst, char *src, size_t n);
clock_t timenow(void);
void getInput(int y, int x, char *buf, int maxlen);
int compare(struct THREADINFO *a, struct THREADINFO *b); 
void list_init(struct LLIST *ll);
int list_insert(struct LLIST *ll, struct THREADINFO *thr_info);
int list_delete(struct LLIST *ll, struct THREADINFO *thr_info);
