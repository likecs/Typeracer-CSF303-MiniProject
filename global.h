/*libraries used in the code*/
#ifndef global
#define global

#include <sys/file.h>
#include <sys/param.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <bits/posix1_lim.h>
#include <ctype.h>
#include <curses.h>
#include <errno.h>
#include <fcntl.h>
#include <dirent.h>
#include <getopt.h>
#include <locale.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <ifaddrs.h>
#include <linux/if_link.h>
#include <netdb.h>
#include <pthread.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include "structDef.h"

/*macros used in the code*/

#define _GNU_SOURCE
#define _(string)	gettext(string)



#define DEFAULT_IP 			"127.0.0.1"
#define BACKLOG 			10
#define BUFFSIZE 			1024
#define ALIASLEN 			32
#define OPTLEN 				16
#define LINEBUFF 			2048
#define MAXCLIENTS			3

#endif
