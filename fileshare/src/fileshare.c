/* Goals for the file-share program
 * 1. access files [COMPLETE]
 * 	1.1 validate the file (file exists, etc) [COMPLETE]
 * 	1.2 make files? [COMPLETE]
 *	1.3 provide filepath to server
 * 2. create a server and a client [COMPLETE]
 * 	* these should be separate modes that the user can choose
 * 	* should use TCP
 * 3. check that user has the proper permissions to access file [COMPLETE]
 * 4. figure out serilization so it is more compatible
 * 5. encryption and compression?
 ******************************5/8 62.5% COMPLETE*****************************
 * SIDE QUESTS:
 * 	- create a packet format so that it doesnt write a bunch of extra data
 */ 
#include "../inc/filehandling.h"
#include "../inc/networking.h"
void initialize_server(void);
void initialize_client(FILE *pfile,char *filename,char *temfilename,bool istemp);
fileinfo FileInfo;
char filename[MAXFNLEN];
struct addrinfo hints, *servinfo, *p;
int sockfd,new_fd, numbytes,rv;
int main(void){
	char mode = 'n';
	fileinfo mainInfo = {.pfile = NULL,.istemp = false, .tempfilename = ""};
	printf("Would you like to run the program in (s)erver mode or (c)lient mode? ");
	if(!scanf(" %c",&mode)){
		fprintf(stderr,"Error reading create choice");
		perror("create choice");
		exit(1);
	}
	switch(mode){
		case 's':
			initialize_server();	
			break;
		case 'c':
			getchar();
			getfilename(filename);
			mainInfo = openfile(mainInfo.pfile,filename);	
			initialize_client(mainInfo.pfile,filename,mainInfo.tempfilename,mainInfo.istemp);
			break;
		default:
			printf("invalid choice\n");
	}
	if(mainInfo.pfile != NULL){ fclose(mainInfo.pfile); }
	return 0;
}
void initialize_server(void){
	int yes = 1;
	char s[INET6_ADDRSTRLEN];
	struct sockaddr_storage their_addr; // connectors address info
	struct sigaction sa;
	socklen_t sin_size;
	memset(&hints,0,sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE; // use my IP
	if((rv = getaddrinfo(NULL,PORT,&hints,&servinfo)) != 0){
		fprintf(stderr,"getaddrinfo: %s\n",gai_strerror(rv));
		exit(1);
	}
	/* loop through all the results and bind to the first one that works */
	for(p = servinfo; p != NULL; p = p->ai_next){
		if((sockfd = socket(p->ai_family,p->ai_socktype,p->ai_protocol)) == -1){
			perror("server: socket");
			continue;
		}
		if(setsockopt(sockfd,SOL_SOCKET,SO_REUSEADDR,&yes,sizeof(int)) == -1){
			perror("setsockopt");
			exit(1);
		}
		if(bind(sockfd,p->ai_addr,p->ai_addrlen) == -1){
			close(sockfd);
			perror("server: bind");
			continue;
		}
		break;
	}
	freeaddrinfo(servinfo);
	if(p == NULL){
		fprintf(stderr,"server: failed to bind\n");
		exit(1);
	}
	if(listen(sockfd,BACKLOG) == -1){
		perror("listen");
		exit(1);
	}
	sa.sa_handler = sigchld_handler; // reap all dead processes
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = SA_RESTART;
	if(sigaction(SIGCHLD,&sa,NULL) == -1){
		perror("sigaction");
		exit(1);
	}
	printf("server: waiting for connections...\n");
	/* this is the main accept loop */
	while(1){
		sin_size = sizeof(their_addr);
		new_fd = accept(sockfd,(struct sockaddr*) &their_addr,&sin_size);
		if(new_fd == -1){
			perror("accept");
			continue;
		}
		inet_ntop(their_addr.ss_family,get_in_addr((struct sockaddr*) &their_addr),s,sizeof(s));
		printf("server: got connection from %s\n",s);
		/* this is the child process that handles the connections themselves */
		if(!fork()){
			close(sockfd); // child doesn't need the listener
			char buf[MAXDATASIZE];
			char filesizestr[MAXFNLEN];
			char confirmationofsize;
			FILE *pfile = NULL;
			int filesize = 0;
			if((numbytes = recv(new_fd,buf,MAXDATASIZE - 1,0)) == -1){
				perror("recv");
				exit(1);
			}
			buf[numbytes] = '\0';
			pfile = fopen(buf,"r");
			if(pfile == NULL){
				if(userpermsverif(filename,1)){
					if(send(new_fd,"FILE NOT FOUND",15,0) == -1){ perror("send"); }
				}
				else{
					if(send(new_fd,"PERMISSION DENIED",18,0) == -1){ perror("send"); }
				}
				break;
			}
			else{
				if((fseek(pfile,0,SEEK_END) != 0)){
					perror("file size");
					break;
				}
				filesize = ftell(pfile);
				snprintf(filesizestr,MAXFNLEN,"%d",filesize);
				if(send(new_fd,filesizestr,sizeof(filesizestr),0) == -1){
					perror("send");
					break;
				}
				if((numbytes = recv(new_fd,&confirmationofsize,1,0)) == -1){
					perror("recv");
					exit(1);
				}
				if(confirmationofsize == 'n'){
					fclose(pfile);
					close(new_fd);
					fprintf(stderr,"server: Transfer cancelled connection closed\n");
					printf("server: waiting for connections...\n");
					exit(1);
				}
				else{
					if(!sendfile(new_fd,&filesize,buf)){
						fclose(pfile);
						close(new_fd);
						fprintf(stderr,"server: failed to send file\n");
						perror("send file");
						printf("server: waiting for connections...\n");
						exit(1);
					}
				}
				fclose(pfile);
				close(new_fd);
				printf("server: Transfer complete! connection closed\n");
				printf("server: waiting for connections...\n");
				exit(0);
			}
		}
		close(new_fd);
	}
}
void initialize_client(FILE *pfile,char filen[],char tempfilename[],bool istemp){
	char buf[MAXDATASIZE] = {0};
	char s[INET6_ADDRSTRLEN] = {0};
	char ip[15] = {0};
	char confirm;
	int size = 0;
	int filesize = 0;
	int permtest = 0;
	for(unsigned short i = 0; i < MAXFNLEN;i++){
		size++;
		if(filen[i] == '\0'){ break; }
	}
	memset(&hints,0,sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	printf("Enter the IP address of the machine you would like to connect to: ");
	if(!fgets(ip,sizeof(ip),stdin)){
		fprintf(stderr,"error reading IP address\n");
		perror("Read IP");
		exit(1);
	}
	for(unsigned short i = 0; i != sizeof(ip); i++){
		if(ip[i] == '\n'){ ip[i] = '\0'; }
	}
	if(strcmp(ip,"localhost") == 0){ strncpy(ip,"127.0.0.1",12); }
	if((rv = getaddrinfo(ip,PORT,&hints,&servinfo)) != 0){
		fprintf(stderr,"getaddrinfo: %s\n",gai_strerror(rv));
		exit(1);
	}
	/* loop through all the results and connect to the first one that works */
	for(p = servinfo; p != NULL; p = p->ai_next){
		if((sockfd = socket(p->ai_family,p->ai_socktype,p->ai_protocol)) == -1){
			perror("client socket");
			continue;
		}
		if(connect(sockfd,p->ai_addr,p->ai_addrlen) == -1){
			close(sockfd);
			perror("client connnect");
			continue;
		}
		break;
	}
	if(p == NULL){
		fprintf(stderr,"client: failed to connect\n");
		freeaddrinfo(servinfo);
		exit(2);
	}
	inet_ntop(p->ai_family,get_in_addr((struct sockaddr*) p->ai_addr),s,sizeof(s));
	printf("client: connecting to %s\n",s);
	freeaddrinfo(servinfo);
	sendfn(sockfd,filen,&size);
	if((numbytes = recv(sockfd,buf,MAXDATASIZE - 1,0)) == -1){
		perror("recv");
		exit(1);
	}
	buf[numbytes] = '\0';
	permtest = strncmp(buf,"PERMISSION DENIED",1);
	if(permtest == 0){
		printf("permission denied server-side, closing program\n");
		if(remove(filen) != 0){
			fprintf(stderr,"failed to remove file\n");
			perror("removing file");
			return;
		}
		if(remove(tempfilename) != 0){
			fprintf(stderr,"failed to remove temp file\n");
			perror("removing file");
			return;
		}
		return;
	}
	printf("client: recieved file size of %s bytes, is this correct(y/n)? ",buf);
	if(!scanf(" %c",&confirm)){
		perror("size confirm");
		exit(1);
	}
	if(send(sockfd,&confirm,sizeof(confirm),0) == -1){
		perror("send confirm");
		exit(1);
	}
	switch(confirm){
		case 'y':
			filesize = atoi(buf);
			bzero(buf,sizeof(buf));
			printf("file size before recvfile: %d\n",filesize);
			if(recvfile(sockfd,pfile,&filesize,istemp) == false){
				close(sockfd);
				fprintf(stderr,"client: failed to recieve file\n");
				perror("recieving file");
				exit(1);
			}
			close(sockfd);
			if(istemp == false){ printf("transfer complete!\nclosing program\n"); }
			else{
				if(!replacetemp(tempfilename,filen)){
					fprintf(stderr,"client: File failed to save to non-temp file\n");
					perror("replace temp");
					exit(1);
				}
			}
			break;
		case 'n':
			printf("filesize refused, closing program\n");
			break;
		default:
			printf("invalid option\n");
	}
}
