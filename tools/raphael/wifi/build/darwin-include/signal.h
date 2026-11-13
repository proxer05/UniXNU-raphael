#ifndef _SIGNAL_H
#define _SIGNAL_H
#include <sys/types.h>
typedef int sig_atomic_t;
void (*signal(int, void (*)(int)))(int);
int raise(int);
#define SIGINT 2
#define SIGTERM 15
#define SIGKILL 9
#define SIGPIPE 13
#define SIGHUP 1
#define SIGALRM 14
#define SIGUSR1 30
#define SIGCHLD 20
#define SIG_IGN ((void (*)(int))1)
#define SIG_DFL ((void (*)(int))0)
#endif
