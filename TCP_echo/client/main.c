#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

#define MAXLINE 1024
#define PORT 50123

void str_cli(FILE* fp, int sockfd);
int readline(int sockfd, void* ptr, int maxline);

int main(int argc, char** argv){
	int sockfd;
	struct sockaddr_in servaddr;

	if(argc != 2){
		perror("usage:tcpcli <IPaddress>");
		exit(1);
	}
	sockfd = socket(AF_INET, SOCK_STREAM, 0);

	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(PORT);
	inet_pton(AF_INET, argv[1], &servaddr.sin_addr);

	if(connect(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr)) == -1){
		perror("connect failed!");
	}
	printf("connect sucess!server port is %d\n",ntohs(servaddr.sin_port));
	str_cli(stdin,sockfd);
	return 0;
}

void str_cli(FILE* fp, int sockfd){
	char sendline[MAXLINE], recvline[MAXLINE];

	while(fgets(sendline,MAXLINE,fp) != NULL){
		write(sockfd, sendline, strlen(sendline));
		readline(sockfd, recvline, MAXLINE);
		printf("from server:");
		fputs(recvline,stdout);
	}
}

int readline(int sockfd, void* vptr, int maxline){
	char c,* ptr=vptr;
	int n;
	for(n=1; n<maxline; n++){
		ssize_t nread = read(sockfd,&c,1);
		if(nread == -1){
			perror("read error;");
			close(sockfd);
			return -1;
		}
		else if(nread = 0){
			printf("success.");
			close(sockfd);
			return n-1;
		}
		else{
			*ptr++ = c;
			if(c == '\n')
				break;
		}
	}
	*ptr = '\0';
	return n;
}
