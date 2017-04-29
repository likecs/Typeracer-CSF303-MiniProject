#include "structDef.h"
#include "utilities.h"
#include "errorHandling.h"

int SERV = 0, CLIENT = 0;
int	wordcount = 0;
int graph = 0;
int position_word[22];
char dictdirectory[MAXPATHLEN];
char strings_word[22][20];
float rate;
int scoreReceive = 0;
int isconnected, aliaslen;
char infoion[LINEBUFF];
struct scores final[MAXCLIENTS+1];
int ply, escpe;
int timerval = 60;
int DEFAULT_PORT = 2048;
int CLIENTS = 1;

int main(int argc, char **argv)
{
	int menuopt;
	char serveraddr[MAXHOSTNAMELEN];
	unsigned long val;
	initialize_curses();

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
	menu_create();
	menuopt = 0;
	do 
	{
		/*come back to inital states */
        SERV = 0; CLIENT = 0;
		wordcount = 0;
		rate = 0;
		scoreReceive = 0;
		isconnected = 0; aliaslen = 0;
		ply = 0;
		escpe = 0;
		timerval = 60;
		DEFAULT_PORT = 2048;
		CLIENTS = 1;
		initializeRules();
		switch (menuopt = getch()) 
		{
			case '1':
				SERV = 1;
				writeServerLog("Server infoion selected");
				startserver();
				closeAllSockets();
				writeServerLog("Reached Main Menu");
				break;
			case '2':
				CLIENT = 1;
				writeClientLog("Client infoion selected");
				startclient();
				close(sockfd);
				writeClientLog("Reached Main Menu");
				break;
			default:
				break;
		}
		SERV = 0;
		CLIENT = 0;
		if (menuopt > '0' && menuopt < '3')
		{
			menu_create();
		}
	} while (menuopt != '3');
	end_curses();
	writeServerLog("Closing TypeRacer");
	return 0;
}
