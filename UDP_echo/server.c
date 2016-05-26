#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <stdlib.h>
#include <stdio.h>
#include <strings.h>

#define IPADDRESS "127.0.0.1"
#define PORT 53421
#define MAXLINE 1024

void dg_echo(int , struct sockaddr*, socklen_t );

int main(int argc, char** argv){
	int sockfd;
	int res;
	struct sockaddr_in servaddr,cliaddr;

	sockfd=socket(AF_INET, SOCK_DGRAM, 0);

	bzero(&servaddr,sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(PORT);
	if((res=inet_pton(AF_INET, IPADDRESS, &servaddr.sin_addr) != 1)){
		perror("ip address error.");
		exit(1);
	}
	bind(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr));

	dg_echo(sockfd, (struct sockaddr*)&cliaddr, sizeof(cliaddr));

	return 0;
}

void dg_echo(int sockfd, struct sockaddr* cliaddr, socklen_t clilen){
	int n;
	char buf[MAXLINE];

	for(;;){
		n = recvfrom(sockfd, buf, MAXLINE, 0, cliaddr, &clilen);
		sendto(sockfd, buf, n, 0, cliaddr, clilen);
	}
}