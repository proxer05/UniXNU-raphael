#ifndef _NETINET_IN_H
#define _NETINET_IN_H
#include <sys/types.h>
struct in_addr { unsigned int s_addr; };
struct sockaddr_in { unsigned char sin_len; unsigned char sin_family; unsigned short sin_port; struct in_addr sin_addr; char sin_zero[8]; };
#define INADDR_ANY 0u
#define INADDR_LOOPBACK 0x7f000001u
#define INADDR_BROADCAST 0xffffffffu
#define IPPROTO_IP 0
#define IPPROTO_TCP 6
#define IPPROTO_UDP 17
#endif
