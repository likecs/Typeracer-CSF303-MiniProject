#include "structDef.h"
#include "utilities.h"
#include "errorHandling.h"

int SERV = 0, CLIENT = 0;
int	wordcount = 0;
int graph = 0;
int wordpos[22];
char worddir[MAXPATHLEN];
char wordstring[22][20];
float rate;
int scoreReceive = 0;
int isconnected, aliaslen;
char option[LINEBUFF];
struct scores final[MAXCLIENTS+1];
int ply, escend;
int timerval = 120;
int DEFAULT_PORT = 2048;
int CLIENTS = 1;

int main(int argc, char **argv)
{
	int wheretogo;
	char serv[MAXHOSTNAMELEN];
	unsigned long val;
	initcursestuff();

	time_t timer;
    char buffer[26];
    struct tm* tm_info;

    time(&timer);
    tm_info = localtime(&timer);

    strftime(buffer, 26, "\n%Y-%m-%d %H:%M:%S\n", tm_info);
   	writeServerLog(buffer);
   	writeClientLog(buffer);
	writeServerLog("Welcome to TypeRacer v1.0");
	writeClientLog("Welcome to TypeRacer v1.0");

	getName();
	drawmenu();
	wheretogo = 0;
	do 
	{
		/*come back to inital states */
        SERV = 0; CLIENT = 0;
		wordcount = 0;
		rate = 0;
		scoreReceive = 0;
		isconnected = 0; aliaslen = 0;
		ply = 0;
		escend = 0;
		timerval = 120;
		DEFAULT_PORT = 2048;
		CLIENTS = 1;
		initializeRules();
		switch (wheretogo = getch()) 
		{
			case '1':
				SERV = 1;
				writeServerLog("Server Option selected");
				startserver();
				closeAllSockets();
				writeServerLog("Reached Main Menu");
				break;
			case '2':
				CLIENT = 1;
				writeClientLog("Client Option selected");
				startclient();
				close(sockfd);
				writeClientLog("Reached Main Menu");
				break;
			default:
				break;
		}
		SERV = 0;
		CLIENT = 0;
		if (wheretogo > '0' && wheretogo < '3')
		{
			drawmenu();
		}
	} while (wheretogo != '3');
	endcursestuff();
	writeServerLog("Closing TypeRacer");
	return 0;
}
