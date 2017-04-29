#include "client.h"

void sendFoundWords_client(char *newword)
{
	int sent;
	struct cmdmsg packet;
	memset(&packet, 0, sizeof(struct cmdmsg));
	strcpy(packet.plyname, "CLIENTFOUND");
	strcpy(packet.buff, newword);

	if(escpe!=2)
	sent = send(sockfd, (void *)&packet, sizeof(struct cmdmsg), 0);
}

int receiveWords()
{
	int bytes;
	struct cmdmsg packet;
	int i;
	while(1)
	{
		bytes = recv(sockfd, (void *)&packet, sizeof(struct cmdmsg), 0);
		if (bytes < 1)
		{
			escpe = 2;
			isconnected = 0;
			writeClientLog("Server diconnected");
			return 0;
		}
		if(!strcmp(packet.buff,"START"))
		{   
			ply = 1;
			timerval = atoi(packet.infoion);
			return 1;
		}
		else if(!strcmp(packet.plyname, "SERVFOUND") && CLIENT)
		{
			clearFoundWord(packet.buff);
			return 2;
		}
		else if(!strcmp(packet.plyname,"SERVWORDS"))
		{
			int slot = atoi(packet.infoion);
			my_strncpy(strings_word[slot], packet.buff, sizeof(strings_word[slot]) - 1);
			position_word[slot] = -1;
			return 3;
		}
		else if(!strcmp(packet.buff, "GAME OVER!"))
		{
			escpe = 1;
			return 4;
		}
		else if(!strcmp(packet.infoion, "SCORES"))
		{
			memset(&final[scoreReceive], 0, sizeof(struct scores));
			strcpy(final[scoreReceive].name, packet.plyname);
			final[scoreReceive].points = atoi(packet.buff);
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
		if (strcmp(input, strings_word[i]))
		{
			continue;
		}
		foundword = 1;
		deleteword(i, position_word[i], strlen(strings_word[i]));
		position_word[i] = -2;
		my_strncpy(strings_word[i], " ", sizeof(strings_word[i]) - 1);
	}
	input[0] = '\0';
}

void *receiver(void *param) 
{
	int recvd;
	struct cmdmsg packet;
	while(!isconnected)
		;
	while(isconnected) 
	{
		receiveWords();
	}
	return NULL;
}

char serveraddr[MAXHOSTNAMELEN];

void startclient()
{
	int exitnow;
	char port[6];
	info.port = DEFAULT_PORT;
	exitnow = 0;
	serveraddr[0] = '\0';
	strcpy(serveraddr , DEFAULT_IP);
	strcpy(port ,"2048");
	do 
	{
		clear();
		mvprintw( 6, 30, _("1. Host: %s"), serveraddr);
		mvprintw( 7, 30, _("2. Port: %s"), port);
		mvaddstr( 8, 30, _("3. Connect "));
		mvaddstr(11, 30, _("Choose: "));
		switch (getch()) 
		{
			case '1':
				mvprintw(11, 30, _("Enter Host: %s"), serveraddr);
				getInput(11, 31 + strlen(_("Enter Host:")), serveraddr, sizeof(serveraddr) - 1);
				serveraddr[sizeof(serveraddr) - 1] = '\0';
				exitnow = 0;
				break;
			case '2':
				mvprintw(11, 30, _("Enter Port: %s"), port);
				getInput(11, 31 + strlen(_("Enter Port:")), port, sizeof(port) - 1);
				port[sizeof(port) - 1] = '\0';
				info.port = strtol(port, NULL, 10);
				if (info.port <= 1024)
				{
					info.port = DEFAULT_PORT;
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
	writeClientLog(serveraddr);
	writeClientLog("Port chosen: ");
	writeClientLog(port);

	clear();
	mvprintw( 5, 30, _("Name: %s"),info.name);
	mvprintw( 6, 30, _("Host: %s"),serveraddr);
	mvprintw( 7, 30, _("Port: %d"),info.port);
	refresh();
	struct tInfo threadinfo;
	pthread_create(&threadinfo.tid, NULL, receiver, (void *)&threadinfo);
	sockfd = connect_with_server();
	writeClientLog("Client connected to server");
	isconnected = 1;
	while(!ply)
		;
	maingameLogic();
}

int connect_with_server() 
{
	int fd_client, errorVal;
	struct sockaddr_in serv_addr;

	if((fd_client = socket(AF_INET, SOCK_STREAM, 0)) == -1) 
	{
		errorVal = errno;
		writeClientLog("Error in creating socket");
		return errorVal;
	}

	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(info.port);
	serv_addr.sin_addr.s_addr = inet_addr(serveraddr);
	memset(&(serv_addr.sin_zero), 0, 8);

	if(connect(fd_client, (struct sockaddr *)&serv_addr, sizeof(struct sockaddr)) == -1) 
	{
		errorVal = errno;
		writeClientLog("Error in connecting to server");
		return errorVal;
	}
	else 
	{
		writeClientLog("Connection successful");
		return fd_client;
	}
}
