#ifndef _ARPA_INET_H
#define _ARPA_INET_H
#include <sys/types.h>
#include <stdint.h>
#include <sys/socket.h>
unsigned int inet_addr(const char *);
int inet_aton(const char *, struct in_addr *);
const char *inet_ntop(int, const void *, char *, socklen_t);
int inet_pton(int, const char *, void *);
uint16_t htons(uint16_t);
uint16_t ntohs(uint16_t);
uint32_t htonl(uint32_t);
uint32_t ntohl(uint32_t);
#endif
