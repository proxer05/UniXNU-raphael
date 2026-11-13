#ifndef _UNISTD_H
#define _UNISTD_H
#include <stddef.h>
#include <stdint.h>
typedef unsigned int uid_t;
typedef unsigned int gid_t;
typedef long off_t;
typedef int pid_t;
typedef int useconds_t;
ssize_t read(int, void *, size_t);
ssize_t write(int, const void *, size_t);
int close(int);
int fcntl(int, int, ...);
off_t lseek(int, off_t, int);
int unlink(const char *);
int rename(const char *, const char *);
int access(const char *, int);
pid_t fork(void);
pid_t getpid(void);
uid_t getuid(void);
int kill(pid_t, int);
unsigned int sleep(unsigned int);
int usleep(useconds_t);
int isatty(int);
char *getcwd(char *, size_t);
int chdir(const char *);
int dup(int);
int dup2(int, int);
int pipe(int *);
int ftruncate(int, off_t);
int setpriority(int, int, int);
#define R_OK 4
#define W_OK 2
#define X_OK 1
#define F_OK 0
#define STDIN_FILENO 0
#define STDOUT_FILENO 1
#define STDERR_FILENO 2
ssize_t pread(int, void *, size_t, off_t);
ssize_t pwrite(int, const void *, size_t, off_t);
#endif
int setiopolicy_np(int, int, int);
int fsync(int);
#define IOPOL_TYPE_DISK 1
#define IOPOL_SCOPE_PROCESS 1
#define IOPOL_IMPORTANT 3
