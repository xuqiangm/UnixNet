#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <string.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define PORT 53421
#define IPADDRESS "127.0.0.1"
#define MAXLINE 1024

void dg_cli(FILE*, int , struct sockaddr*, socklen_t);

int main(int argc, char** argv){
	int sockfd;
	struct sockaddr_in servaddr;
	int res;

	sockfd=socket(AF_INET, SOCK_DGRAM, 0);

	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(PORT);
	if((res = inet_pton(AF_INET, IPADDRESS, &servaddr.sin_addr)) != 1){
		perror("IP address error.");
		exit(1);
	}

	dg_cli(stdin, sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr));

	return 0;
}

void dg_cli(FILE* fp, int sockfd, struct sockaddr* servaddr, socklen_t servlen){
	int n;
	char sendline[MAXLINE],recvline[MAXLINE];

	while(fgets(sendline, MAXLINE, fp) != NULL){
		sendto(sockfd, sendline, strlen(sendline), 0, servaddr, servlen);

		n = recvfrom(sockfd, recvline, MAXLINE, 0 ,NULL, NULL);

		recvline[n] = '\0';
		fputs(recvline, stdout);
	}
}