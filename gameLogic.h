#ifndef gameLogic
#define gameLogic

#include "structDef.h"

#endif

int insertword(void);
void deleteword(int y, int x, size_t length);
static void checkWords(void);
static void word_animate(void);
static void checkInput(int *escpe, char *input, clock_t *stime, unsigned *char_position, int *wordset, int *wrongwrds);
int maingameLogic(void);
void cleartrie(void);
int filltrie(char *filename);
int comp_function(const void * a, const void * b);
