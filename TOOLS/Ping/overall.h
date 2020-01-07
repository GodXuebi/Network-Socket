#ifndef __OVERALL_H__
#define __OVERALL_H__
#define BUFFERSIZE 72
#define K 1023
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/time.h>
#include <netinet/ip_icmp.h>
#include <netinet/ip.h>
#include <netinet/in.h>
#include <errno.h>
#include <netdb.h>
#include <sys/socket.h>
#include <signal.h>
#include "overall.h"
typedef struct pingm_packet
{
	struct timeval tv_begin;
	struct timeval tv_end;
	short int seq;
	int flag;
}pingm_packet;
extern int rawsock;
extern short packet_send;
extern short packet_recv;
extern pid_t pid;
extern short alive;
extern pingm_packet pingpacket[128];
extern struct timeval tv_begin,tv_end,tv_interval;
extern unsigned char send_buff[BUFFERSIZE];//72
extern unsigned char recv_buff[2*K];
extern struct sockaddr_in dest;
extern char dest_str[80];
extern short alive;
#endif
