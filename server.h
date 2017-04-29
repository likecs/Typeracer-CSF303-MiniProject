#ifndef server
#define server

#include "structDef.h"

#endif

void startserver(void);
void showInterfaces();
void initializeServ();
void *clientThread(void *fd); 
void sendStart();
void sendGameOver();
void sendWords(int slot, char newword[] );
void sendFoundWords_server(char newword[]);
void closeAllSockets();
