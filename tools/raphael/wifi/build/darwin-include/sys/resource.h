#ifndef _SYS_RESOURCE_H
#define _SYS_RESOURCE_H
#include <sys/types.h>
#define RLIMIT_NOFILE 8
#define RLIM_INFINITY ((rlim_t)-1)
typedef unsigned long rlim_t;
struct rlimit { rlim_t rlim_cur; rlim_t rlim_max; };
int getrlimit(int, struct rlimit *);
int setrlimit(int, const struct rlimit *);
#endif
#define PRIO_PROCESS 0
#define PRIO_DARWIN_PROCESS 4
