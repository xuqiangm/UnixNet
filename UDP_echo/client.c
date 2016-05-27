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
char * sock_ntop(const struct sockaddr *sa, socklen_t salen);

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
	char sendline[MAXLINE],recvline[MAXLINE+1];
	socklen_t len;
	struct sockaddr* preply_addr;

	preply_addr = malloc(servlen);

	while(fgets(sendline, MAXLINE, fp) != NULL){
		sendto(sockfd, sendline, strlen(sendline), 0, servaddr, servlen);

		n = recvfrom(sockfd, recvline, MAXLINE, 0 ,preply_addr, &len);
		if(len != servlen || memcmp(servaddr, preply_addr, len)!=0){
			printf("reply from %s (ignored)\n", sock_ntop(preply_addr, len));
			continue;
		}

		recvline[n] = '\0';
		fputs(recvline, stdout);
	}
}

char * sock_ntop(const struct sockaddr *sa, socklen_t salen)
{
    char		portstr[8];
    static char str[128];		/* Unix domain is largest */

	switch (sa->sa_family) {
		case AF_INET: {
			struct sockaddr_in	*sin = (struct sockaddr_in *) sa;

			if (inet_ntop(AF_INET, &sin->sin_addr, str, sizeof(str)) == NULL)
				return(NULL);
			if (ntohs(sin->sin_port) != 0) {
				snprintf(portstr, sizeof(portstr), ":%d", ntohs(sin->sin_port));
				strcat(str, portstr);
			}
			return(str);
		}
		return NULL;
	}
}