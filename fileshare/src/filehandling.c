#include "../inc/filehandling.h"
char tempfiletemplate[20]  = "tempXXXXXX";
fileinfo Finfo;
void getfilename(char* fn){
	int fnlen = 0;
	printf("Enter the name of the file you would like to use:\n");
	do{
		if(!fgets(fn,MAXFNLEN,stdin)){
			fprintf(stderr,"error reading filename\n");
			perror("Read fname");
			exit(1);
		}
		fnlen = strnlen(fn,MAXFNLEN);
		if(fnlen <= 1){
			printf("filename must be at least one character\n");
		}
	}while(fnlen <= 1);
	for(unsigned short i = 0; i != MAXFNLEN; i++){
		if(fn[i] == '\n'){
			fn[i] = '\0';
		}
	}
}
fileinfo openfile(FILE *pfile,char *filen){
	char createfile = 'n';
	pfile = fopen(filen,"r");
	if(!pfile){
		printf("Would you like to create the file (y/n)? ");
		if(!scanf(" %c",&createfile)){
			fprintf(stderr,"Error reading create choice");
			perror("create choice");
			exit(1);
		}
		if(createfile == 'y'){
			pfile = fopen(filen,"w+");
			Finfo.pfile = pfile;
			Finfo.istemp = false;
			getchar();
			return Finfo;
		}	
		else{
			printf("Closing program\n");
			exit(1);
		}
	}
	else{
		if(!userpermsverif(filen,0)){
			fprintf(stderr,"cilent: user doesn't have permission to access file\n");
			perror("client perms");
			exit(1);
		}
		fclose(pfile);
		if(!mkstemp(tempfiletemplate)){
			fprintf(stderr,"failed to create temp file\n");
			perror("temp file make");
			exit(1);
		}
		printf("mkstemp: %s\n",tempfiletemplate);
		Finfo.tempfilename = tempfiletemplate;
		pfile = fopen(tempfiletemplate,"w");
		if(!pfile){
			fprintf(stderr,"Error opening temp file\n");
			perror("temp file open");
			exit(1);
		}
		Finfo.pfile = pfile;
		Finfo.istemp = true;
		return Finfo;
	}
}
bool replacetemp(char *tempfile,char *filen){
	char filenamecpy[MAXFNLEN] = {0};
	for(unsigned short i = 0; i < MAXFNLEN; i++){
		filenamecpy[i] = filen[i];
	}
	if(remove(filen) != 0){
		fprintf(stderr,"failed to remove file\n");
		perror("removing file");
		return false;
	}
	if(rename(tempfile,filenamecpy) == -1){
		fprintf(stderr,"failed to rename file\n");
		perror("rename file");
		return false;
	}
	return true;
}
bool userpermsverif(char *filen,int type){
	int accessstatusread = 0;
	int accessstatuswrite = 0;
	switch(type){
		case 0:
		accessstatusread = access(filen,R_OK);
		accessstatuswrite = access(filen,W_OK);
		if((accessstatusread != 0 || accessstatuswrite != 0)){
			return false;
		}
		else{
			return true;
		}
		break;
		case 1:
			accessstatusread = access(filen,R_OK);
			if(accessstatusread != 0){
				return false;
			}
			else{
				return true;
			}
			break;
		default:
			fprintf(stderr,"Something has gone very wrong to get here\n");
			exit(1);
	}
}
