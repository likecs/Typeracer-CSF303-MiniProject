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

int chooseSettings(void)
{
	int exitnow;
	char defcli[6];
	char defport[6];
	char deftimer[6];
	char cli[6];
	char port[6];
	char timer[6];
	opt.port = DEFAULT_PORT;
	exitnow = 0;
	cli[0] = '3';
	cli[1] = '\0';
	timer[0] = '6';timer[1]='0';timer[2]='\0';
	strcpy(port ,"2048");
	strcpy(deftimer, timer);
	strcpy(defport, port);
	strcpy(defcli, cli);
	do 
	{
		clear();
		mvprintw( 6, 30, _("1. No. of Players (Max 3 players excluding you) : %s"), cli);
		mvprintw( 7, 30, _("2. Port: %s"), port);
		mvprintw( 8, 30, _("3. Timer for each game: %s seconds"), timer);
		mvaddstr( 9, 30, _("4. Connect "));
		mvaddstr(11, 30, _("Choose: "));
		switch (getch()) 
		{
			case '1':
				mvprintw(11, 30, _("Number of Players: %s"), cli);
				getInput(11, 31 + strlen(_("Number of Players:")), cli, sizeof(cli) - 1);
				cli[sizeof(cli) - 1] = '\0';
				CLIENTS = strtol(cli, NULL, 10);
				if (CLIENTS > 3)
				{
					CLIENTS = 3;
					strcpy(cli, defcli);
				}
				exitnow = 0;
				break;
			case '2':
				mvprintw(11, 30, _("Enter Port: %s"), port);
				getInput(11, 31 + strlen(_("Enter Port:")), port, sizeof(port) - 1);
				port[sizeof(port) - 1] = '\0';
				opt.port = strtol(port, NULL, 10);
				if (opt.port <= 1024)
				{
					opt.port = DEFAULT_PORT;
					strcpy(port, defport);
				}
				exitnow = 0;
				break;
			case '3':
				mvprintw(11, 30, _("Enter Timer Value: %s"), timer);
				getInput(11, 31 + strlen(_("Enter Timer Value:")), timer, sizeof(timer) - 1);
				timer[sizeof(timer) - 1] = '\0';
				timerval = strtol(timer, NULL, 10);
				if (timerval > 300)
				{
					timerval = 60;
					strcpy(timer, deftimer);
				}
				exitnow = 0;
				break;
			case '4':
				exitnow = 1;
			default:
				break;
		}
	} while (!exitnow);
	return 0;
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