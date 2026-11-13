#ifndef _SYS_STAT_H
#define _SYS_STAT_H
#include <sys/types.h>
#include <stdint.h>
#include <stddef.h>
typedef unsigned int uid_t;
typedef unsigned int gid_t;
struct stat { dev_t st_dev; ino_t st_ino; mode_t st_mode; nlink_t st_nlink; uid_t st_uid; gid_t st_gid; long st_size; };
int stat(const char *, struct stat *);
int fstat(int, struct stat *);
int mkdir(const char *, mode_t);
#define S_IFMT 0170000
#define S_IFDIR 0040000
#define S_IFREG 0100000
#define S_ISDIR(m) (((m) & S_IFMT) == S_IFDIR)
#define S_ISREG(m) (((m) & S_IFMT) == S_IFREG)
#define S_IRWXU 0000700
#define S_IRWXG 0000070
#define S_IRWXO 0000007
int chmod(const char *, mode_t);
int fchmod(int, mode_t);
#endif
