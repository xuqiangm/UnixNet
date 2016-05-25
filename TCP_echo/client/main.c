#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include <sys/socket.h>
#include <sys/types.h>
#include <sys/select.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

#include "../lib/algorithm.h"

#define true 1
#define false 0
#define MAXLINE 1024
#define PORT 50123

void str_cli(FILE* fp, int sockfd);
int readline(int sockfd, void* ptr, int maxline);

int main(int argc, char** argv){
	int sockfd,i;
	struct sockaddr_in servaddr;

	if(argc != 2){
		perror("usage:client <IPaddress>");
		exit(1);
	}

	sockfd = socket(AF_INET, SOCK_STREAM, 0);

	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(PORT);
	inet_pton(AF_INET, argv[1], &servaddr.sin_addr);

	if(connect(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr)) == -1){
		perror("connect failed!");
		exit(1);
	}
	printf("connect sucess!server port is %d\n",ntohs(servaddr.sin_port));
	str_cli(stdin,sockfd);
	return 0;
}

void str_cli(FILE* fp, int sockfd){
	fd_set rset;
	char buf[MAXLINE];
	int n;
	int maxfdp1,stdineof = 0;

	FD_ZERO(&rset);

	while(1){
		if(stdineof == 0)
			FD_SET(fileno(fp), &rset);
		FD_SET(sockfd, &rset);
		maxfdp1 = max(fileno(fp),sockfd)+1;
		select(maxfdp1, &rset, NULL, NULL, NULL);

		if(FD_ISSET(sockfd, &rset)){	//套接字可读
			if((n = read(sockfd, buf, MAXLINE)) == 0){
				if(stdineof == 1)	//正常终止
					return ;
				else{
					printf("str_cli:服务器进程终止\n");
					exit(1);
				}
			}
			write(fileno(stdout), buf, n);
		}

		if(FD_ISSET(fileno(fp),&rset)){	//输入可读
			if((n = read(fileno(fp), buf, MAXLINE)) == 0){
				stdineof = 1;
				shutdown(sockfd, SHUT_WR);
				FD_CLR(fileno(fp),&rset);
				continue;
			}
			else{
				write(sockfd, buf, n);
			}
		}
	}
	return ;
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
