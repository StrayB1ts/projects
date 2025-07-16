/* Define some default packets as well as general packet structure in here... or at least the max sizes for the different packet fields */
#include "../inc/common.h"
#ifndef NETWORKING_H
#define NETWORKING_H
#define PACKETSIZE_FIELD 8
#define CRC_SIZE 4
#define PACKET_PADDING 0x0
#define MAXDATASIZE (1024 - PACKETSIZE_FIELD - CRC_SIZE)
#define BACKLOG 10
#define PORT "42069"
typedef struct Packet Packet;
struct Packet{
	unsigned int packet_size : PACKETSIZE_FIELD * sizeof(int);
	void *databuffer[MAXDATASIZE];
	unsigned int crc : CRC_SIZE * sizeof(short);
};
void *get_in_addr(struct sockaddr *sa);
void sigchld_handler(int s);
int sendfn(int sock, char *buf, int *len);
bool sendfile(int sock,int *filesize, char *filename);
bool recvfile(int sock,FILE *pfile,int *filesize,bool istemp);
// fill these in later
bool make_packet(void *buffer);
bool deconst_packet(int size,void *buffer, int crc);
int compute_crc(int size,void *buffer);
#endif
