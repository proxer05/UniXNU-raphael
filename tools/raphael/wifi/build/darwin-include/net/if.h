#define IFNAMSIZ 16
#ifndef _NET_IF_H
#define _NET_IF_H
#include <sys/types.h>
struct if_data { u_char ifi_type; u_char ifi_physical; u_char ifi_addrlen; u_char ifi_hdrlen; u_char ifi_recvquota; u_char ifi_xmitquota; u_long ifi_mtu; u_long ifi_metric; u_long ifi_baudrate; u_long ifi_ipackets; u_long ifi_ierrors; u_long ifi_opackets; u_long ifi_oerrors; u_long ifi_collisions; u_long ifi_ibytes; u_long ifi_obytes; u_long ifi_imcasts; u_long ifi_omcasts; u_long ifi_iqdrops; u_long ifi_noproto; u_long ifi_lastchange; };
struct if_msghdr { unsigned short ifm_msglen; unsigned char ifm_version; unsigned char ifm_type; int ifm_addrs; int ifm_flags; unsigned short ifm_index; struct if_data ifm_data; };
#define RTM_IFINFO 0xe
#endif
