#ifndef _POLL_H
#define _POLL_H
typedef unsigned int nfds_t;
struct pollfd { int fd; short events; short revents; };
int poll(struct pollfd *, nfds_t, int);
#define POLLIN 0x0001
#define POLLOUT 0x0004
#define POLLERR 0x0008
#define POLLHUP 0x0010
#define POLLNVAL 0x0020
#endif
