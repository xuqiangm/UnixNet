//此为书中第七章 7.3 小结的程序
#include <netinet/tcp.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

union val{
	int i_val;
	long l_val;
	struct linger linger_val;
	struct timeval timeval_val;
}val;

static char* sock_str_flag(union val* , int);
static char* sock_str_int(union val*, int);
static char* sock_str_linger(union val*, int);
static char* sock_str_timeval(union val*, int);

struct sock_opts{
	const char* opt_str;
	int opt_level;
	int opt_name;
	char* (*opt_val_str)(union val*, int);
}sock_opts[]={
#ifdef SO_BROADCAST
	{"SO_BROADCAST", SOL_SOCKET, SO_BROADCAST, sock_str_flag},
#else
	{"SO_BROADCAST", 0, 0, NULL},
#endif
	{NULL, 0, 0, NULL}
};

int main(int argc, char** argv){
	int fd;
	socklen_t len;
	struct sock_opts* ptr;

	for(ptr=sock_opts; ptr->opt_str != NULL; ptr++){
		printf("%s:",ptr->opt_str);
		if(ptr->opt_val_str == NULL)
			printf("undefined\n");
		else{
			switch(ptr->opt_level){
				case SOL_SOCKET:
				case IPPROTO_IP:
				case IPPROTO_TCP:
					fd=socket(AF_INET, SOCK_STREAM, 0);
					break;
			#ifdef	IPV6
				case IPPROTO_IPV6:
					fd = socket(AF_INET6, SOCK_STREAM, 0);
					break;
			#endif
			#ifdef	IPPROTO_SCTP
				case IPPROTO_SCTP:
					fd = socket(AF_INET, SOCK_SEQPACKET, IPPROTO_SCTP);
					break;
			#endif
				default:
					perror("error.");
					exit(1);
			}
		}
		len = sizeof(val);
		if(getsockopt(fd,ptr->opt_level, ptr->opt_name, &val, &len) == -1){
			perror("getsocket error.");
			exit(1);
		}
		else{
			printf("default = %s\n",(*ptr->opt_val_str)(&val,len));
		}
		close(fd);
	}
	return 0;
}


static char strres[128];

static char* sock_str_flag(union val* ptr,int len){
	if(len != sizeof(int))
		snprintf(strres,sizeof(strres),"size (%d) not sizeof(int)",len);
	else
		snprintf(strres,sizeof(strres),"%s",(ptr->i_val == 0)?"off":"on");
	return strres;
}

static char* sock_str_int(union val* ptr,int len){
	if(len != sizeof(int))
		snprintf(strres,sizeof(strres),"size (%d) not sizeof(int)",len);
	else
		snprintf(strres,sizeof(strres),"%s","int");
	return strres;
}

static char* sock_str_linger(union val* prtr, int len){
	if(len != sizeof(struct linger))
		snprintf(strres,sizeof(strres),"size (%d) not sizeof(linger)",len);
	else
		snprintf(strres,sizeof(strres),"linger");
	return strres;
}

static char* sock_str_timeval(union val* prtr, int len){
	if(len != sizeof(struct timeval))
		snprintf(strres,sizeof(strres),"size (%d) not sizeof(timeval)",len);
	else
		snprintf(strres,sizeof(strres),"timeval");
	return strres;
}