#ifndef client
#define client

#include "structDef.h"

#endif


void sendFoundWords_client(char *newword);
int receiveWords();
void clearFoundWord(char *input);
void *receiver(void *param);
void startclient();
int connect_with_server(); 
