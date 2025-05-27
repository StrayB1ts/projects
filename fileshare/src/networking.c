#include "../inc/networking.h"
void *get_in_addr(struct sockaddr *sa){
	/* return the IP address of the socket passed, works with IPv4 and IPv6 */
	if(sa->sa_family == AF_INET){
		return &(((struct sockaddr_in*) sa) -> sin_addr);
	}
	return &(((struct sockaddr_in6*) sa) -> sin6_addr);
}
void sigchld_handler(int s){
	/* waitpid() might overwrite errno, so we save and restore it */
	int saved_errno = errno;
	while(waitpid(-1,NULL,WNOHANG) > 0)
	errno = saved_errno;
}
int sendfn(int sock, char *buf, int *len){
	int total = 0; // how many bytes have been sent
	int bytesleft = *len; // how many are left to send
	int n = 0;
	while(total < *len){
		n = send(sock,buf + total, bytesleft,0);
		if(n == -1){
			break;
		}
		total += n;
		bytesleft -= n;
	}
	*len = total; // return number of bytes actually sent
	return n == -1 ? -1 : 0; // return -1 on failure and 0 on success
}
bool sendfile(int sock,int *filesize,char *filen){
	int bytessent = 0;
	int amountread = 0;
	int amountcopy = 0;
	FILE *pfile = NULL;
	char buffer[MAXDATASIZE];
	pfile = fopen(filen,"r");
	if(pfile == NULL){
		fprintf(stderr,"failed to open file in sendfile function\n");
		perror("open file");
		return false;
	}
	fseek(pfile,0,SEEK_SET);
	while(bytessent < *filesize){
		amountread += fread(&buffer,1,sizeof(buffer) - 1,pfile);
		printf("amount read before check: %d\n",amountread);
		if(amountread == 0){
			fprintf(stderr,"server: error reading file\n");
			perror("reading file");
			fclose(pfile);
			return false;
		}
		amountcopy = amountread;
		if(amountread != *filesize){
			amountread += fread(buffer,1,sizeof(buffer) - 1,pfile);
			if(amountread == amountcopy){
				fprintf(stderr,"server: error reading rest of file\n");
				perror("reading file");
				fclose(pfile);
				return false;
			}
		}
		bytessent += send(sock,&buffer,sizeof(buffer),0);
		printf("bytes sent: %d\n",bytessent);
		if(bytessent == -1){
			fprintf(stderr,"server: error sending file\n");
			perror("sending file");
			fclose(pfile);
			return false;
		}
		((*filesize - bytessent) > 0) ? printf("sending file... %d bytes left\n",*filesize - bytessent) : printf(" ");
		bzero(buffer,sizeof(buffer));
	}
	fclose(pfile);
	return true;
}
bool recvfile(int sock,FILE *pfile,int *filesize,bool istemp){
	int bytesrecv = 0;
	char buffer[MAXDATASIZE];
	printf("file size in recvfile: %d buffer in recvfile: %s\n",*filesize,buffer);
	while(bytesrecv < *filesize){
		printf("while loop started\n");
		bytesrecv += recv(sock,&buffer,sizeof(buffer),0);	
		if(bytesrecv == 0){
			fprintf(stderr,"client: failed to recieve initial file data\n");
			perror("initial data");
			fclose(pfile);
			return false;
		}
		printf("buffer before write and clear: %s\n",buffer);
		fwrite(buffer,sizeof(buffer),1,pfile);
		bzero(buffer,sizeof(buffer));
	}
	return true;
}
