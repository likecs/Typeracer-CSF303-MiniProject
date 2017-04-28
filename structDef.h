#ifndef structDef
#define structDef

#include "global.h"

struct stats 
{
	uint8_t level;
	uint32_t score;
	uint32_t tcount;
	uint32_t wordswritten;
	float ratio;
	float speed;
	float totalspeed;
	clock_t duration;
	unsigned int sinit;
	char name[21];
} now;

struct opt 
{
	int cheat;
	int net;
	int port;
	unsigned int seed;
	int usecolors;
	char name[21];
} opt;

struct rules 
{
	int label;
	int minwords;
	int maxwords;
	int minspeed;
	int maxspeed;
	int step;
	int smooth;
	float spd_multi;
	char name[31];
	unsigned long minscore;
	char fname[FILENAME_MAX + 1];
} rules;

struct rawdata 
{
	char *bulk;
	char **word;
	size_t n;
	size_t max;
} words;

struct scores 
{
	char name[ALIASLEN];
	int score;
};

struct PACKET 
{
	char option[OPTLEN]; // instruction
	char alias[ALIASLEN]; // client's alias
	char buff[BUFFSIZE]; // payload
};

struct THREADINFO 
{
	pthread_t thread_ID; // thread's pointer
	int sockfd; // socket file descriptor
	char alias[ALIASLEN]; // client's alias
};

struct USER 
{
	int sockfd; // user's socket descriptor
	char alias[ALIASLEN]; // user's name
};

struct LLNODE 
{
	struct THREADINFO threadinfo;
	struct LLNODE *next;
};

struct LLIST 
{
	struct LLNODE *head, *tail;
	int size;
};

/*Common Variables used throught the code*/

extern int SERV, CLIENT;
extern int	 wordcount;
extern int  graph;
extern int  wordpos[22];
extern float rate;
extern char worddir[MAXPATHLEN];
extern char wordstring[22][20];
extern int scoreReceive;
extern int isconnected, aliaslen;
extern int ply;
extern char option[LINEBUFF];
extern struct scores final[MAXCLIENTS+1];
extern int escend;
extern int timerval;
extern int DEFAULT_PORT;
extern int CLIENTS;

int sockfd, newfd;
struct THREADINFO thread_info[MAXCLIENTS];
struct LLIST client_list;
pthread_mutex_t clientlist_mutex;

#endif