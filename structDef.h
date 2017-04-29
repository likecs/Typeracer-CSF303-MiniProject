#ifndef structDef
#define structDef

#include "global.h"

struct info 
{
	int port;
	int num;
	unsigned int seed;
	char name[21];
} info;

struct constrnts 
{
	int label;
	int mnwrds;
	int mxwrds;
	int mnspeed;
	int mxspeed;
	int step;
	int smooth;
	float spd_multi;
	char name[31];
	unsigned long minscore;
	char fname[FILENAME_MAX + 1];
} constrnts;

struct gameInfo 
{
	uint8_t lvl;
	uint32_t points;
	uint32_t numofwrds;
	float typoratio;
	float game_cps;
	float game_tcps;
	clock_t duration;
	float topspd;
	unsigned int sinit;
	char name[21];
} curr_stat;

struct dictionary 
{
	char *rawwords;
	char **word;
	size_t n;
	size_t max;
} wrds;

struct scores 
{
	char name[ALIASLEN];
	int points;
};

struct cmdmsg 
{
	char infoion[infoLEN]; // instruction
	char plyname[ALIASLEN]; // client's plyname
	char buff[BUFFSIZE]; // payload
};

struct tInfo 
{
	pthread_t tid; // thread's pointer
	int sockfd; // socket file descriptor
	char plyname[ALIASLEN]; // client's plyname
};

struct listnode 
{
	struct tInfo threadinfo;
	struct listnode *next;
};

struct list 
{
	struct listnode *head, *tail;
	int size;
};

/*Common Variables used throught the code*/

extern int SERV, CLIENT;
extern int	 wordcount;
extern int  graph;
extern int  position_word[22];
extern float rate;
extern char dictdirectory[MAXPATHLEN];
extern char strings_word[22][20];
extern int scoreReceive;
extern int isconnected, aliaslen;
extern int ply;
extern char infoion[LINEBUFF];
extern struct scores final[MAXCLIENTS+1];
extern int escpe;
extern int timerval;
extern int DEFAULT_PORT;
extern int CLIENTS;

int sockfd, fd_client;
struct tInfo thread_info[MAXCLIENTS];
struct list info_clients;
pthread_mutex_t mutex_clients;

#endif