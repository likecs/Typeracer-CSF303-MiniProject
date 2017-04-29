#ifndef menuModel
#define menuModel

#include "structDef.h"

#endif


void getName();
void menu_create(void);
void screen_create(void);
void status_create(unsigned char_position);
void end_curses(void);
void initialize_curses(void);
int chooseSettings(void);
int choosewordfile(void);
int fileselmenu(void);
void pressanykey(int y, int x);
