#include <sys/socket.h>
#include <sys/types.h>
#include <sys/select.h>
#include <sys/time.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <signal.h>

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

#include "../lib/utility.h"

#define IPADDRESS "127.0.0.1"
#define PORT 50123
#define MAXLINE 1024

void str_echo(int connfd);

int main(int argc,char** argv){
	int listenfd,connfd,sockfd;
	int client[MAXLINE];
	int maxi, maxfd;
	int nready;
	int i;
	ssize_t n;
	int len;
	char buf[MAXLINE];
	fd_set rset,allset;

	for(i=0;i<MAXLINE;i++)
		client[i] = -1;

	struct sockaddr_in cliaddr,servaddr;
	listenfd = socket(AF_INET,SOCK_STREAM,0);

	bzero(&servaddr,sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(PORT);
	if(!inet_pton(AF_INET, IPADDRESS, &servaddr.sin_addr)){
		perror("ip format error.");
		exit(1);
	}
	if(bind(listenfd,(struct sockaddr*)&servaddr,sizeof(servaddr)) == -1){
		perror("bind error");
		exit(1);
	}

	listen(listenfd,10);

	FD_ZERO(&allset);
	FD_SET(listenfd, &allset);
	maxi = -1;
	maxfd = listenfd;

	for(;;){
		rset = allset;
		nready = select(maxfd+1, &rset, NULL, NULL, NULL);

		if(FD_ISSET(listenfd, &rset)){	//监听套接字可读
			connfd = accept(listenfd, NULL, NULL);
			printf("get a client\n");
			for(i=0; i<FD_SETSIZE; i++){	//找到一个client位置插入该文件描述符
				if(client[i] < 0){
					client[i] = connfd;
					break;
				}
			}
			if(i == FD_SETSIZE){
				printf("too many clients.");
			}
			FD_SET(connfd,&allset);
			if(connfd > maxfd)
				maxfd = connfd;
			if(i > maxi)
				maxi = i;
			if(--nready <= 0)
				continue;
		}

		for(i=0; i<=maxi; i++){
			if((sockfd=client[i]) < 0)
				continue;
			if(FD_ISSET(sockfd, &rset)){	//注意此处不能使用 allset，因为如果allset有以为可能是因为监听字可读后设置的
											//而不是因为select函数设置的
				if((n=read(sockfd, buf,MAXLINE)) == 0){	//连接被用户关闭
					close(sockfd);
					FD_CLR(sockfd, &allset);	//修改时使用 allset
					client[i]=-1;
				}
				else{
					while(n > 0){
						len = write(sockfd,buf,n);
						n -= len;
					}
				}
			}
			if(--nready <= 0)
				break;
		}
	}
	return 0;
}

void str_echo(int connfd){
	char buf[1024];
	size_t n;
	while((n = read(connfd, buf, sizeof(buf))) > 0){
		printf("from client: %.*s\n",(int)n,buf);
		write(connfd, buf, n);
	}
}