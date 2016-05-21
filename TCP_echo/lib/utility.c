#include "./utility.h"

//包裹 signal 函数


Sigfunc* Signal(int signo, Sigfunc* func){
	struct sigaction act, oact;

	act.sa_handler = func;
	sigemptyset(&act.sa_mask);
	act.sa_flags = 0;

	//下面的这段代码不是很懂
	if(signo == SIGALRM){
#ifdef SA_INTERRUPT
		act.sa_flags |= SA_INTERRUPT;
#endif
	}else{
#ifdef SA_RESTART
		act.sa_flags |= SA_RESTART;
#endif
	}

	if(sigaction(signo, &act, &oact) < 0)
		return (SIG_ERR);
	return (oact.sa_handler);
}

//处理信号 SIGCHLD
void sig_chld(int signo){
	pid_t pid;
	int stat;
	while((pid = waitpid(-1, &stat, WNOHANG)) > 0)
		printf("child %d terminated\n", pid);
	return ;
}