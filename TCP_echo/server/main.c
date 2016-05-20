#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

#define IPADDRESS "127.0.0.1"
#define PORT 50123

void str_echo(int connfd);

int main(int argc,char** argv){
	int listenfd,connfd;
	pid_t childpid;
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

	int clilen = sizeof(cliaddr);
	while(1){
		connfd = accept(listenfd, (struct sockaddr*)&cliaddr, &clilen);
		printf("connect sucess!server port is %d\n",ntohs(servaddr.sin_port));
		childpid = fork();
		if(childpid == 0){	//child process
			close(listenfd);
			str_echo(connfd);
			exit(0);
		}
		close(connfd);
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