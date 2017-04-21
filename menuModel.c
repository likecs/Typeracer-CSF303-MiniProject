#include "menuModel.h"

void getName()
{
	clear();
	mvaddstr( 3, 30, "TypeRacer v1.0 ");
	mvaddstr( 5, 30, _("Enter Player Name: "));
	mvaddstr(5, 30, _("Enter Player Name: "));
	getInput(5, 31 + strlen(_("Enter Player Name: ")), opt.name, sizeof(opt.name) - 1);
	opt.name[sizeof(opt.name) - 1] = '\0';
	writeServerLog("Welcome ");
	writeServerLog(opt.name);

}

void drawmenu(void)
{
	clear();
	mvaddstr( 3, 30, "TypeRacer v1.0 ");
	mvaddstr( 5, 30, _("1. Start a Server"));
	mvaddstr( 6, 30, _("2. Connect to Server"));
	mvaddstr( 7, 30, _("3. Quit"));
	mvaddstr(13, 30, _("Choose: "));
}

void drawscreen(void)
{
	clear();
	move(22, 0);
	hline(ACS_HLINE, 80);
	mvaddch(23, 1, '>');
	mvaddch(23, 21, '<');
	drawstatus(0);
	mvaddstr(23, 38, _("Score:"));
	mvaddstr(23, 49, _("tCPS:"));
	mvaddstr(23, 60, _("cCPS:"));
	mvaddstr(23, 72, _("TIME:"));
}

void drawstatus(unsigned inputpos)
{
	mvprintw(23, 45, "%d", now.score);
	mvprintw(23, 55, "%2.2f", now.totalspeed);
	mvprintw(23, 66, "%2.2f", now.speed);
	mvprintw(23, 77, "%d ", timerval);
	move(23, 2 + inputpos);
	move(23, 2 + inputpos);
}

void endcursestuff(void)
{
	if (!graph)
	{
		return;
	}
	clear();
	refresh();
	endwin();
	graph = 0;
}

void initcursestuff(void)
{
	if (graph)
	{
		return;
	}
	initscr();
	graph = 1;
	keypad(stdscr, TRUE);
	noecho();
	cbreak();
	nodelay(stdscr, FALSE);
	flushinp();
}

int choosewordfile(void)
{
	int a, k;
	writeServerLog("Choose Dictonary: ");

	k = fileselmenu();
	if (k < 1) 
	{
		clear();
		mvaddstr(1, 5, _("No word lists found..."));
		pressanykey(3, 5);
		return -1;
	}
	return k;
}

int fileselmenu(void)
{
	int a, cpos, k;
	clear();
	mvaddstr(2, 5, _("Dictionaries :"));
	mvaddstr(4, 5, _("1. C Sharp"));
	mvaddstr(5, 5, _("2. English"));
	mvaddstr(6, 5, _("3. French"));
	mvaddstr(7, 5, _("4. DOS"));
	mvaddstr(8, 5, _("5. Programming"));
	mvaddstr(9, 5, _("6. Unix"));
	mvaddstr(12, 5, _("Choose a dictionary :"));
	char c;
	c = getch();
	while(c < '1' || c > '6')
	{
		c = getch();
	}
	k = c-'0';
	return k;
}

void pressanykey(int y, int x)
{
	flushinp();
	mvaddstr(y, x, _("Press any key to continue..."));
	getch();
}