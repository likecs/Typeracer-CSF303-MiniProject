#ifndef gameLogic
#define gameLogic

#include "structDef.h"

#endif

int addnewword(void);
void clearword(int y, int x, size_t length);
static void cwords(void);
static void movewords(void);
static void parseinput(int *escend, char *input, clock_t *starttime, unsigned *inputpos, int *nappaykset, int *wrngret);
int play(void);
void freewords(void);
int loadwords(char *filename);
int cmpfunc(const void * a, const void * b);
