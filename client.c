#include "client.h"

void sendFoundWords_client(char *newword)
{
	int sent;
	struct PACKET packet;
	memset(&packet, 0, sizeof(struct PACKET));
	strcpy(packet.alias, "CLIENTFOUND");
	strcpy(packet.buff, newword);
	/* send request to close this connetion */
	sent = send(sockfd, (void *)&packet, sizeof(struct PACKET), 0);
}

int receiveWords()
{
	int bytes;
	struct PACKET packet;
	int i;
	while(i = recv(sockfd, (void *)&packet, sizeof(struct PACKET), MSG_PEEK))
	{
		if(i < 1)
		{
			return 0;
		}
		bytes = recv(sockfd, (void *)&packet, sizeof(struct PACKET), 0);
		if(!strcmp(packet.buff,"START"))
		{   
			ply = 1;
			return 1;
		}
		else if(!strcmp(packet.alias, "SERVFOUND") && CLIENT)
		{
			clearFoundWord(packet.buff);
			return 2;
		}
		else if(!strcmp(packet.alias,"SERVWORDS"))
		{
			int slot = atoi(packet.option);
			my_strncpy(wordstring[slot], packet.buff, sizeof(wordstring[slot]) - 1);
			wordpos[slot] = -1;
			return 3;
		}
		else if(!strcmp(packet.buff, "GAME OVER!"))
		{
			escend = 1;
			return 4;
		}
		else if(!strcmp(packet.option, "SCORES"))
		{
			memset(&final[scoreReceive], 0, sizeof(struct scores));
			strcpy(final[scoreReceive].name, packet.alias);
			final[scoreReceive].score = atoi(packet.buff);
			scoreReceive++;
		}
	}
	return 0;
}

void clearFoundWord(char *input)
{
	int foundword = 0,i;
	for (i = 0; i < 22; i++) 
	{
		if (strcmp(input, wordstring[i]))
		{
			continue;
		}
		foundword = 1;
		clearword(i, wordpos[i], strlen(wordstring[i]));
		wordpos[i] = -2;
		my_strncpy(wordstring[i], " ", sizeof(wordstring[i]) - 1);
	}
	input[0] = '\0';
}

void *receiver(void *param) 
{
	int recvd;
	struct PACKET packet;
	while(!isconnected)
		;
	while(isconnected) 
	{
		receiveWords();
	}
	return NULL;
}

char serv[MAXHOSTNAMELEN];

void startclient()
{
	int exitnow;
	char port[6];
	opt.port = DEFAULT_PORT;
	exitnow = 0;
	serv[0] = '\0';
	strcpy(serv , DEAFAULT_IP);
	strcpy(port ,"2048");
	do 
	{
		clear();
		mvprintw( 6, 30, _("1. Host: %s"), serv);
		mvprintw( 7, 30, _("2. Port: %s"), port);
		mvaddstr( 8, 30, _("3. Connect "));
		mvaddstr(11, 30, _("Choose: "));
		switch (getch()) 
		{
			case '1':
				mvprintw(11, 30, _("Enter Host: %s"), serv);
				getInput(11, 31 + strlen(_("Enter Host:")), serv, sizeof(serv) - 1);
				serv[sizeof(serv) - 1] = '\0';
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
				}
				exitnow = 0;
				break;
			case '3':
				exitnow = 1;
			default:
				break;
		}
	} while (!exitnow);

	writeClientLog("Client started");
	writeClientLog("Host chosen: ");
	writeClientLog(serv);
	writeClientLog("Port chosen: ");
	writeClientLog(port);

	clear();
	mvprintw( 5, 30, _("Name: %s"),opt.name);
	mvprintw( 6, 30, _("Host: %s"),serv);
	mvprintw( 7, 30, _("Port: %d"),opt.port);
	refresh();
	struct THREADINFO threadinfo;
	pthread_create(&threadinfo.thread_ID, NULL, receiver, (void *)&threadinfo);
	sockfd = connect_with_server();
	writeClientLog("Client connected to server");
	isconnected = 1;
	while(!ply)
		;
	play();
}

int connect_with_server() 
{
	int newfd, err_ret;
	struct sockaddr_in serv_addr;
	/* open a socket */
	if((newfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) 
	{
		err_ret = errno;
		writeClientLog("Error in creating socket");
		return err_ret;
	}
	/* set initial values */
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(opt.port);
	serv_addr.sin_addr.s_addr = inet_addr(serv);
	memset(&(serv_addr.sin_zero), 0, 8);
	/* try to connect with server */
	if(connect(newfd, (struct sockaddr *)&serv_addr, sizeof(struct sockaddr)) == -1) 
	{
		err_ret = errno;
		writeClientLog("Error in connecting to server");
		return err_ret;
	}
	else 
	{
		writeClientLog("Connection successful");
		return newfd;
	}
}
