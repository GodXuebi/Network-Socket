#ifndef __PACKET_H__
#define __PACKET_H__
#include "overall.h"
extern struct timeval icmp_tvsub(struct timeval*end,struct timeval*begin);
extern void icmp_pack(struct icmp*icmph,int seq,int length);
extern int icmp_unpack(char*buf,int len);
extern unsigned short icmp_chksum(unsigned short *addr,int len);
extern pingm_packet*icmp_findpacket(int seq);
extern void icmp_statistic(void);
extern void icmp_usage(void);
extern void icmp_statistics(void);
extern void icmp_sigint(int signo);
extern void*icmp_recv(void*argv);
extern void*icmp_send(void*argv);
#endif
