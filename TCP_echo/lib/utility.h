#include <signal.h>
#include <sys/wait.h>
#include <stdio.h>

typedef void Sigfunc (int);
Sigfunc* Signal(int signo, Sigfunc* func);

void sig_chld(int signo);