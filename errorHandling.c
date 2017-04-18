#include "errorHandling.h"

void writeServerLog(char *inp)
{
	FILE *fp;
	fp = fopen("log_server.txt","a");
	fprintf(fp, "%s\n", inp);
	fclose(fp);
}

void writeClientLog(char *inp)
{
	FILE *fp;
	fp = fopen("log_client.txt","a");
	fprintf(fp, "%s\n", inp);
	fclose(fp);
}