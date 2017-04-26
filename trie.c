#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <malloc.h>
#include <stdbool.h>
#include <assert.h>

#define ALPHA 		26
#define MAX			1000

char data[MAX][ALPHA];

struct node 
{
	bool leaf;
	struct node *c[ALPHA];
};

typedef struct node *trie;

trie newNode() 
{
	trie go = (trie)malloc(sizeof(struct node));
	int i;
	go->leaf = false;
	for(i = 0; i < ALPHA; ++i) 
	{
		go->c[i] = NULL;
	}
	return go;
}

void insert(trie root, const char *s) 
{
	int len = strlen(s), i, idx;
	trie go = root;
	for(i = 0; i < len; ++i) 
	{
		idx = s[i] - 'a';
		if (go->c[idx] == NULL) 
		{
			go->c[idx] = newNode();
		}
		go = go->c[idx];
	}
	go->leaf = true;
}

bool search(trie root, const char *s) 
{
	int len = strlen(s), i, idx;
	trie go = root;
	for(i = 0; i < len; ++i) 
	{
		idx = s[i] - 'a';
		if (go->c[idx] == NULL) 
		{
			return false;
		}
		go = go->c[idx];
	}	
	return (go != NULL && go->leaf);
}

void initaliseTrie(char *filename)
{
	FILE *fp = fopen(filename, "r");
	trie root = newNode();
	//ignore name of files
	fscanf(fp, "%s", data[0]);
	fscanf(fp, "%s", data[0]);
	for(int i = 0; i < MAX; ++i) 
	{
		fscanf(fp, "%s", data[i]);
		insert(root, data[i]);
	}
	fclose(fp);
}