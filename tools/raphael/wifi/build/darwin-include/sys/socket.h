#ifndef _SYS_SOCKET_H
#define _SYS_SOCKET_H
#include <stddef.h>
#include <netinet/in.h>
typedef unsigned int socklen_t;
struct sockaddr { unsigned char sa_len; unsigned char sa_family; char sa_data[14]; };
struct msghdr { void *msg_name; socklen_t msg_namelen; struct iovec *msg_iov; int msg_iovlen; void *msg_control; socklen_t msg_controllen; int msg_flags; };
struct iovec { void *iov_base; size_t iov_len; };
struct cmsghdr { socklen_t cmsg_len; int cmsg_level; int cmsg_type; };
int socket(int, int, int);
int bind(int, const struct sockaddr *, socklen_t);
int connect(int, const struct sockaddr *, socklen_t);
int send(int, const void *, size_t, int);
int sendto(int, const void *, size_t, int, const struct sockaddr *, socklen_t);
ssize_t recv(int, void *, size_t, int);
ssize_t recvfrom(int, void *, size_t, int, struct sockaddr *, socklen_t *);
int sendmsg(int, const struct msghdr *, int);
ssize_t recvmsg(int, struct msghdr *, int);
int setsockopt(int, int, int, const void *, socklen_t);
int getsockopt(int, int, int, void *, socklen_t *);
int shutdown(int, int);
#define SOCK_STREAM 1
#define SOCK_DGRAM 2
#define SOCK_RAW 3
#define AF_UNSPEC 0
#define AF_UNIX 1
#define AF_INET 2
#define PF_UNSPEC AF_UNSPEC
#define PF_UNIX AF_UNIX
#define PF_INET AF_INET
#define PF_SYSTEM 16
#define AF_SYSTEM 16
#define SYSPROTO_CONTROL 2
#define AF_SYS_CONTROL 2
#define SOCK_SEQPACKET 5
#define SOL_SOCKET 0xffff
#define SO_REUSEADDR 0x0004
#define SO_BINDTODEVICE 0x0008
#define MSG_OOB 0x1
#define MSG_PEEK 0x2
#define MSG_DONTWAIT 0x80
#define CMSG_FIRSTHDR(m) ((m)->msg_controllen >= sizeof(struct cmsghdr) ? (struct cmsghdr *)(m)->msg_control : (struct cmsghdr *)0)
#define CMSG_DATA(c) ((void *)((char *)(c) + sizeof(struct cmsghdr)))
int listen(int, int);
int accept(int, struct sockaddr *, socklen_t *);
#endif
