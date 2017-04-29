#include "server.h"

void startserver(void)
{
	chooseSettings();
	int k = 0;
	k = choosewordfile();
	if(k == -1)
	{
		return;
	}
	switch(k) 
	{
		case 1:
			filltrie("dictionaries/words.csharp");
			break;
		case 2:
			filltrie("dictionaries/words.eng");
			break;
		case 3:
			filltrie("dictionaries/words.fra");
			break;
		case 4:
			filltrie("dictionaries/words.dos");
			break;
		case 5:
			filltrie("dictionaries/words.prog");
			break;
		case 6:
			filltrie("dictionaries/words.unix");
			break;
	}
	char *mp[6] = {"Csharp", "English", "French", "DOS", "Programming", "Unix"};
	clear();
	mvprintw(2, 5, _("Server is up and running on PORT %d..."), info.port);
	mvaddstr(3, 5, _("Available Interfaces..."));
	showInterfaces();
	mvprintw(8, 5, _("Chosen Dictionary: %s"), mp[k-1]);
	writeServerLog(mp[k-1]);
	refresh();
	initializeServ();
	sendStart();
	maingameLogic();
	return;
}

void showInterfaces()
{
	struct ifaddrs *ifaddr, *ifa;
	int family, s, n, i=0;
	char host[NI_MAXHOST];
	if (getifaddrs(&ifaddr) == -1) 
	{
		perror("getifaddrs");
		return;
	}
	for (ifa = ifaddr, n = 0; ifa != NULL; ifa = ifa->ifa_next, n++) 
	{
		if (ifa->ifa_addr == NULL)
		{
			continue;
		}
		family = ifa->ifa_addr->sa_family;
		if (family == AF_INET) 
		{
			s = getnameinfo(ifa->ifa_addr, (family == AF_INET) ? sizeof(struct sockaddr_in) : sizeof(struct sockaddr_in6),
							host, NI_MAXHOST, NULL, 0, NI_NUMERICHOST);
			if (s != 0) 
			{
				writeServerLog("getnameinfo() failed:");
				writeServerLog(" %s\n", gai_strerror(s));
				return;
			}
			mvprintw(5+i, 5, _("%-20s address: <%s>"), ifa->ifa_name, host);
			i++;
		}
	}
	writeServerLog("All interfaces displayed");

	freeifaddrs(ifaddr);
}

void initializeServ()
{
	int errorVal, sizesin;
	struct sockaddr_in serv_addr, client_addr;
	 
	initList(&info_clients);
	 
	pthread_mutex_init(&mutex_clients, NULL);
	 
	if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) 
	{
		errorVal = errno;
		return errorVal;
	}
	 
    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &(int){ 1 }, sizeof(int)) < 0)
    {
    	writeServerLog("Error in SO_REUSEADDR");
    	errorVal=errno;
    	return errorVal;
	}
	if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEPORT, &(int){ 1 }, sizeof(int)) < 0)
	{
    	writeServerLog("Error in SO_REUSEADDR");
    	errorVal=errno;
    	return errorVal;
	}
	 
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(info.port);
	serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	memset(&(serv_addr.sin_zero), 0, 8);
	 
	if(bind(sockfd, (struct sockaddr *)&serv_addr, sizeof(struct sockaddr)) == -1) 
	{
		errorVal = errno;
		writeServerLog("Error in binding");
		return errorVal;
	}
	 
	if(listen(sockfd, BACKLOG) == -1) 
	{
		errorVal = errno;
		writeServerLog("Error in listening");
		return errorVal;
	}
	 
	while(1) 
	{
		sizesin = sizeof(struct sockaddr_in);
		if((fd_client = accept(sockfd, (struct sockaddr *)&client_addr, (socklen_t*)&sizesin)) == -1) 
		{
			errorVal = errno;
			writeServerLog("Error in accepting connections");
			return errorVal;
		}
		else 
		{
			struct tInfo threadinfo;
			threadinfo.sockfd = fd_client;
			pthread_mutex_lock(&mutex_clients);
			insertList(&info_clients, &threadinfo);
			pthread_mutex_unlock(&mutex_clients);
			pthread_create(&threadinfo.tid, NULL, clientThread, (void *)&threadinfo);
			if(info_clients.size == CLIENTS) 
			{
				break;
			}
		}
	}
	writeServerLog("Requried Clients Connected");
	mvprintw(11, 5, _("Players Connected: %d / %d\t\t\t\t"),info_clients.size, CLIENTS);
	pressanykey(12,5);
}

void *clientThread(void *fd) 
{
	struct tInfo threadinfo = *(struct tInfo *)fd;
	struct cmdmsg packet;
	struct listnode *curr;
	int bytes, sent;
	while(1) 
	{
		bytes = recv(threadinfo.sockfd, (void *)&packet, sizeof(struct cmdmsg), 0);
		if(!bytes) 
		{
			writeServerLog("Client Disconnected");
			pthread_mutex_lock(&mutex_clients);
			deleteList(&info_clients, &threadinfo);
			pthread_mutex_unlock(&mutex_clients);
			break;
		}
		if(!strcmp(packet.plyname, "CLIENTFOUND"))
		{
			sendFoundWords_server(packet.buff);
			clearFoundWord(packet.buff);
		}
		else if(!strcmp(packet.infoion, "SCORES"))
		{
			memset(&final[scoreReceive], 0, sizeof(struct scores));
			strcpy(final[scoreReceive].name, packet.plyname);
			final[scoreReceive].points = atoi(packet.buff);
			scoreReceive++;
			pthread_mutex_lock(&mutex_clients);
			for(curr = info_clients.head; curr != NULL; curr = curr->next) 
			{
				struct cmdmsg spacket;
				memset(&spacket, 0, sizeof(struct cmdmsg));
				if(!compare(&curr->threadinfo, &threadinfo))
				{
					continue;
				}
				strcpy(spacket.infoion, "SCORES");
				strcpy(spacket.plyname, packet.plyname);
				strcpy(spacket.buff, packet.buff);
				sent = send(curr->threadinfo.sockfd, (void *)&spacket, sizeof(struct cmdmsg), 0);
			}
			pthread_mutex_unlock(&mutex_clients);
		}
	}
	 
	close(threadinfo.sockfd);
	return NULL;
}

void sendStart()
{
	struct cmdmsg packet;
	struct listnode *curr;
	int bytes, sent;

	writeServerLog("Sending Game Start commands");
	pthread_mutex_lock(&mutex_clients);
	for(curr = info_clients.head; curr != NULL; curr = curr->next) 
	{
		struct cmdmsg spacket;
		memset(&spacket, 0, sizeof(struct cmdmsg));
		sprintf(spacket.infoion, "%d", timerval);
		 
		strcpy(spacket.plyname, "SERVER");
		strcpy(spacket.buff, "START");
		sent = send(curr->threadinfo.sockfd, (void *)&spacket, sizeof(struct cmdmsg), 0);
	}
	pthread_mutex_unlock(&mutex_clients);
	writeServerLog("Game Start commands sent");
}

void sendGameOver()
{
	struct cmdmsg packet;
	struct listnode *curr;
	int bytes, sent;
	pthread_mutex_lock(&mutex_clients);
	for(curr = info_clients.head; curr != NULL; curr = curr->next) 
	{
		struct cmdmsg spacket;
		memset(&spacket, 0, sizeof(struct cmdmsg));
		strcpy(spacket.infoion, "msg");
		strcpy(spacket.plyname, "SERVER");
		strcpy(spacket.buff, "GAME OVER!");
		sent = send(curr->threadinfo.sockfd, (void *)&spacket, sizeof(struct cmdmsg), 0);
	}
	pthread_mutex_unlock(&mutex_clients);
}

void sendWords(int slot, char newword[] )
{
	struct cmdmsg packet;
	struct listnode *curr;
	char bffr[BUFFSIZE];
	snprintf(bffr, sizeof(bffr), "%d", slot);
	int bytes, sent;
	pthread_mutex_lock(&mutex_clients);
	for(curr = info_clients.head; curr != NULL; curr = curr->next) 
	{
		struct cmdmsg spacket;
		memset(&spacket, 0, sizeof(struct cmdmsg));
		strcpy(spacket.infoion, bffr);
		strcpy(spacket.plyname, "SERVWORDS");
		strcpy(spacket.buff, newword);
		sent = send(curr->threadinfo.sockfd, (void *)&spacket, sizeof(struct cmdmsg), 0);
	}
	pthread_mutex_unlock(&mutex_clients);
}

void sendFoundWords_server(char newword[])
{
	struct cmdmsg packet;
	struct listnode *curr;
	int bytes, sent;
	pthread_mutex_lock(&mutex_clients);
	for(curr = info_clients.head; curr != NULL; curr = curr->next) 
	{
		struct cmdmsg spacket;
		memset(&spacket, 0, sizeof(struct cmdmsg));
		strcpy(spacket.plyname, "SERVFOUND");
		strcpy(spacket.buff, newword);
		sent = send(curr->threadinfo.sockfd, (void *)&spacket, sizeof(struct cmdmsg), 0);
	}
	pthread_mutex_unlock(&mutex_clients);
}



void closeAllSockets()
{
	struct listnode *curr;
	pthread_mutex_lock(&mutex_clients);
	for(curr = info_clients.head; curr != NULL; curr = curr->next) 
	{
		close(curr->threadinfo.sockfd);
	}
	pthread_mutex_unlock(&mutex_clients);
}
