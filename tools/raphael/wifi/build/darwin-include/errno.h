#ifndef _ERRNO_H
#define _ERRNO_H
int *__error(void);
#define errno (*__error())
#define EACCES 13
#define EAUTH 80
#define EINVAL 22
#define ENOMEM 12
#define ENOENT 2
#define EIO 5
#define EAGAIN 35
#define ENODEV 19
#define EPERM 1
#define ENOBUFS 105
#define EWOULDBLOCK EAGAIN
#define ETIMEDOUT 60
#define ECONNREFUSED 61
#define EHOSTUNREACH 65
#define ENETDOWN 50
#define EINTR 4
#define EBADF 9
#define EMSGSIZE 90
#define ENOTCONN 57
#define EALREADY 37
#define EINPROGRESS 36
#define EOPNOTSUPP 102
#define ENOMEM 12
#define EFAULT 14
#define EBUSY 16
#define EEXIST 17
#define EISDIR 21
#define ENOTDIR 20
#define ENFILE 23
#define EMFILE 24
#define ENOSPC 28
#define ESPIPE 29
#define EROFS 30
#define EPIPE 32
#endif
