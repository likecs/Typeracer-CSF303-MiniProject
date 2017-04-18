#include "server.h"


void startserver(void)
{
	int k = 0;
	k = choosewordfile();
	if(k == -1)
	{
		return;
	}
	switch(k) 
	{
		case 1:
			loadwords("dictionaries/words.csharp");
			break;
		case 2:
			loadwords("dictionaries/words.eng");
			break;
		case 3:
			loadwords("dictionaries/words.fra");
			break;
		case 4:
			loadwords("dictionaries/words.dos");
			break;
		case 5:
			loadwords("dictionaries/words.prog");
			break;
		case 6:
			loadwords("dictionaries/words.unix");
			break;
	}
	char *mp[6] = {"Csharp", "English", "French", "DOS", "Programming", "Unix"};
	clear();
	mvaddstr(2, 5, _("Server is up and running on PORT 2048..."));
	mvaddstr(3, 5, _("Available Interfaces..."));
	showInterfaces();
	mvprintw(8, 5, _("Chosen Dictionary: %s"), mp[k-1]);
	writeServerLog(mp[k-1]);
	refresh();
	initserver();
	sendStart();
	play();
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
				printf("getnameinfo() failed: %s\n", gai_strerror(s));
				return;
			}
			mvprintw(5+i, 5, _("%s\t\taddress: <%s>"), ifa->ifa_name, host);
			i++;
		}
	}
	 writeServerLog("All interfaces displayed");

	freeifaddrs(ifaddr);
}

void initserver()
{
	int err_ret, sin_size;
	struct sockaddr_in serv_addr, client_addr;
	pthread_t interrupt;
	/* initialize linked list */
	list_init(&client_list);
	/* initiate mutex */
	pthread_mutex_init(&clientlist_mutex, NULL);
	/* open a socket */
	if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) 
	{
		err_ret = errno;
		return err_ret;
	}
	/*socket reuse problems*/
    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &(int){ 1 }, sizeof(int)) < 0)
    {
    	writeServerLog("Error in SO_REUSEADDR");
    	err_ret=errno;
    	return err_ret;
	}
	if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEPORT, &(int){ 1 }, sizeof(int)) < 0)
	{
    	writeServerLog("Error in SO_REUSEADDR");
    	err_ret=errno;
    	return err_ret;
	}
	/* set initial values */
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(DEFAULT_PORT);
	serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	memset(&(serv_addr.sin_zero), 0, 8);
	/* bind address with socket */
	if(bind(sockfd, (struct sockaddr *)&serv_addr, sizeof(struct sockaddr)) == -1) 
	{
		err_ret = errno;
		writeServerLog("Error in binding");
		return err_ret;
	}
	/* start listening for connection */
	if(listen(sockfd, BACKLOG) == -1) 
	{
		err_ret = errno;
		writeServerLog("Error in listening");
		return err_ret;
	}
	/* keep accepting connections */
	while(1) 
	{
		sin_size = sizeof(struct sockaddr_in);
		if((newfd = accept(sockfd, (struct sockaddr *)&client_addr, (socklen_t*)&sin_size)) == -1) 
		{
			err_ret = errno;
			writeServerLog("Error in accepting connections");
			return err_ret;
		}
		else 
		{
			struct THREADINFO threadinfo;
			threadinfo.sockfd = newfd;
			strcpy(threadinfo.alias, "Anonymous");
			pthread_mutex_lock(&clientlist_mutex);
			list_insert(&client_list, &threadinfo);
			pthread_mutex_unlock(&clientlist_mutex);
			pthread_create(&threadinfo.thread_ID, NULL, client_handler, (void *)&threadinfo);
			if(client_list.size == CLIENTS) 
			{
				break;
			}
		}
	}
	writeServerLog("Requried Clients Connected");
	mvprintw(11, 5, _("Players Connected: %d / 2\t\t\t\t"),client_list.size);
	pressanykey(12,5);
}

void *client_handler(void *fd) 
{
	struct THREADINFO threadinfo = *(struct THREADINFO *)fd;
	struct PACKET packet;
	struct LLNODE *curr;
	int bytes, sent;
	while(1) 
	{
		bytes = recv(threadinfo.sockfd, (void *)&packet, sizeof(struct PACKET), 0);
		if(!bytes) 
		{
			writeServerLog("Client Disconnected");
			pthread_mutex_lock(&clientlist_mutex);
			list_delete(&client_list, &threadinfo);
			pthread_mutex_unlock(&clientlist_mutex);
			break;
		}
		if(!strcmp(packet.alias, "CLIENTFOUND"))
		{
			sendFoundWords_server(packet.buff);
			clearFoundWord(packet.buff);
		}
		else if(!strcmp(packet.option, "SCORES"))
		{
			memset(&final[scoreReceive], 0, sizeof(struct scores));
			strcpy(final[scoreReceive].name, packet.alias);
			final[scoreReceive].score = atoi(packet.buff);
			scoreReceive++;
			pthread_mutex_lock(&clientlist_mutex);
			for(curr = client_list.head; curr != NULL; curr = curr->next) 
			{
				struct PACKET spacket;
				memset(&spacket, 0, sizeof(struct PACKET));
				if(!compare(&curr->threadinfo, &threadinfo))
				{
					continue;
				}
				strcpy(spacket.option, "SCORES");
				strcpy(spacket.alias, packet.alias);
				strcpy(spacket.buff, packet.buff);
				sent = send(curr->threadinfo.sockfd, (void *)&spacket, sizeof(struct PACKET), 0);
			}
			pthread_mutex_unlock(&clientlist_mutex);
		}
	}
	/* clean up */
	close(threadinfo.sockfd);
	return NULL;
}

void sendStart()
{
	struct PACKET packet;
	struct LLNODE *curr;
	int bytes, sent;

	writeServerLog("Sending Game Start commands");
	pthread_mutex_lock(&clientlist_mutex);
	for(curr = client_list.head; curr != NULL; curr = curr->next) 
	{
		struct PACKET spacket;
		memset(&spacket, 0, sizeof(struct PACKET));
		strcpy(spacket.option, "msg");
		strcpy(spacket.alias, "SERVER");
		strcpy(spacket.buff, "START");
		sent = send(curr->threadinfo.sockfd, (void *)&spacket, sizeof(struct PACKET), 0);
	}
	pthread_mutex_unlock(&clientlist_mutex);
	writeServerLog("Game Start commands sent");
}

void sendGameOver()
{
	struct PACKET packet;
	struct LLNODE *curr;
	int bytes, sent;
	pthread_mutex_lock(&clientlist_mutex);
	for(curr = client_list.head; curr != NULL; curr = curr->next) 
	{
		struct PACKET spacket;
		memset(&spacket, 0, sizeof(struct PACKET));
		strcpy(spacket.option, "msg");
		strcpy(spacket.alias, "SERVER");
		strcpy(spacket.buff, "GAME OVER!");
		sent = send(curr->threadinfo.sockfd, (void *)&spacket, sizeof(struct PACKET), 0);
	}
	pthread_mutex_unlock(&clientlist_mutex);
}

void sendWords(int slot, char newword[] )
{
	struct PACKET packet;
	struct LLNODE *curr;
	char buf[BUFFSIZE];
	snprintf(buf, sizeof(buf), "%d", slot);
	int bytes, sent;
	pthread_mutex_lock(&clientlist_mutex);
	for(curr = client_list.head; curr != NULL; curr = curr->next) 
	{
		struct PACKET spacket;
		memset(&spacket, 0, sizeof(struct PACKET));
		strcpy(spacket.option, buf);
		strcpy(spacket.alias, "SERVWORDS");
		strcpy(spacket.buff, newword);
		sent = send(curr->threadinfo.sockfd, (void *)&spacket, sizeof(struct PACKET), 0);
	}
	pthread_mutex_unlock(&clientlist_mutex);
}

void sendFoundWords_server(char newword[])
{
	struct PACKET packet;
	struct LLNODE *curr;
	int bytes, sent;
	pthread_mutex_lock(&clientlist_mutex);
	for(curr = client_list.head; curr != NULL; curr = curr->next) 
	{
		struct PACKET spacket;
		memset(&spacket, 0, sizeof(struct PACKET));
		strcpy(spacket.alias, "SERVFOUND");
		strcpy(spacket.buff, newword);
		sent = send(curr->threadinfo.sockfd, (void *)&spacket, sizeof(struct PACKET), 0);
	}
	pthread_mutex_unlock(&clientlist_mutex);
}



void closeAllSockets()
{
	struct LLNODE *curr;
	pthread_mutex_lock(&clientlist_mutex);
	for(curr = client_list.head; curr != NULL; curr = curr->next) 
	{
		close(curr->threadinfo.sockfd);
	}
	pthread_mutex_unlock(&clientlist_mutex);
}
