/* Define some default packets as well as general packet structure in here... or at least the max sizes for the different packet fields */
#include "../inc/common.h"
#ifndef NETWORKING_H
#define NETWORKING_H
#define MAXDATASIZE 1024
#define BACKLOG 10
#define PORT "42069"
void *get_in_addr(struct sockaddr *sa);
void sigchld_handler(int s);
int sendfn(int sock, char *buf, int *len);
bool sendfile(int sock,int *filesize, char *filename);
bool recvfile(int sock,FILE *pfile,int *filesize,bool istemp);
// fill these in later
void make_packet(void);
void deconst_packet(void);
#endif
