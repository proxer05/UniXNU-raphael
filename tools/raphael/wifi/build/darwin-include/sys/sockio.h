#ifndef _SYS_SOCKIO_H
#define _SYS_SOCKIO_H
#include <sys/ioccom.h>
struct ifreq { char ifr_name[16]; union { struct sockaddr ifru_addr; struct sockaddr ifru_broadaddr; struct sockaddr ifru_netmask; short ifru_flags; int ifru_metric; int ifru_mtu; char ifru_data[16]; } ifr_ifru; };
#define ifr_addr ifr_ifru.ifru_addr
#define ifr_flags ifr_ifru.ifru_flags
#define ifr_mtu ifr_ifru.ifru_mtu
#define SIOCGIFCONF _IOWR('i', 36, struct ifconf)
#define SIOCGIFFLAGS _IOWR('i', 17, struct ifreq)
#define SIOCSIFFLAGS _IOW('i', 16, struct ifreq)
#define SIOCGIFMTU _IOWR('i', 51, struct ifreq)
struct ifconf { int ifc_len; union { char *ifcu_buf; struct ifreq *ifcu_req; } ifc_ifcu; };
#define ifc_buf ifc_ifcu.ifcu_buf
#define ifc_req ifc_ifcu.ifcu_req
#define IFF_UP 0x1
#define IFF_LOOPBACK 0x8
#define IFF_POINTOPOINT 0x10
struct ifaliasreq { char ifra_name[16]; struct sockaddr ifra_addr; struct sockaddr ifra_broadaddr; struct sockaddr ifra_mask; };
#define SIOCAIFADDR _IOW('i', 26, struct ifaliasreq)
#define SIOCDIFADDR _IOW('i', 25, struct ifreq)
#endif
