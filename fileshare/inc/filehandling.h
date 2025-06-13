#include "../inc/common.h"
#ifndef FILEHANDLING_H
#define FILEHANDLING_H
#define MAXFNLEN 100
typedef struct fileinfo fileinfo;
struct fileinfo{
	FILE *pfile;
	char *tempfilename;
	bool istemp;
};
void getfilename(char*);
fileinfo openfile(FILE *pfile,char *filename);
bool replacetemp(char *tempfile,char *filename);
bool userpermsverif(char *filename,int type);
#endif
