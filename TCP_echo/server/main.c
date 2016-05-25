#include <sys/socket.h>
#include <sys/types.h>
#include <sys/select.h>
#include <sys/time.h>
#include <sys/stropts.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <signal.h>
#include <poll.h>

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

#include "../lib/utility.h"

#define IPADDRESS "127.0.0.1"
#define PORT 50123
#define MAXLINE 1024
#define MAXQUE 10
#define OPEN_MAX 100

void str_echo(int connfd);

int main(int argc,char** argv){
	int i,maxi,listenfd,connfd,sockfd;
	int nready;
	int res, leftn;
	ssize_t n;
	char buf[MAXLINE];
	struct pollfd  clients[OPEN_MAX];
	struct sockaddr_in servaddr;

	listenfd = socket(AF_INET, SOCK_STREAM, 0);

	bzero(&servaddr,sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	if((res = inet_pton(AF_INET, IPADDRESS, &servaddr.sin_addr)) == 0){
		perror("IP address error");
		exit(1);
	}
	servaddr.sin_port = htons(PORT);

	if((res=bind(listenfd, (struct sockaddr*)&servaddr, sizeof(servaddr)))!=0){
		perror("bind() failed.");
		exit(1);
	}

	listen(listenfd, MAXQUE);

	clients[0].fd = listenfd;
	clients[0].events = POLLRDNORM;
	for(i=1; i<OPEN_MAX; i++)
		clients[i].fd = -1;
	maxi = 0;

	for(;;){
		nready = poll(clients, maxi+1, -1);
		if(clients[0].revents & POLLRDNORM){	//有新的客户端链接
			connfd = accept(listenfd, NULL, NULL);
			for(i=1; i<OPEN_MAX; i++){
				if(clients[i].fd == -1){
					clients[i].fd = connfd;
					break;
				}
			}
			if(i == OPEN_MAX){
				perror("too many clients");
				exit(1);
			}
			printf("a new client\n");
			clients[i].events=POLLRDNORM;
			if(i > maxi)
				maxi = i;
			if(--nready <= 0)
				continue;
		}
		for(i = 1; i<=maxi; i++){	//查看所有的已链接的客户
			if((sockfd = clients[i].fd)<0)
				continue;
			if(clients[i].events & (POLLRDNORM | POLLERR)){
				if((n=read(sockfd,buf,MAXLINE)) < 0){
					if(errno == ECONNRESET){
						//客户发送reset
						close(sockfd);
						clients[i].fd = -1;
					}
					else{
						perror("read error.");
						exit(1);
					}
				}
				else if(n == 0){
					//连接被客户关闭
					printf("client close.\n");
					close(sockfd);
					clients[i].fd = -1;
				}
				else{
					write(fileno(stdout), buf, n);
					leftn = n;
					while(leftn > 0){
						leftn -= write(sockfd, buf, leftn);
					}
				}
				if(--nready <= 0)
					break;
			}
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