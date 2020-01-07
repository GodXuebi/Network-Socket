#include "overall.h"
int rawsock=0;
short packet_send=0;
short packet_recv=0;
pid_t pid=0;
short alive=0;
pingm_packet pingpacket[128];
struct timeval tv_begin,tv_end,tv_interval;
unsigned char send_buff[BUFFERSIZE];//72
unsigned char recv_buff[2*K];
struct sockaddr_in dest;
char dest_str[80];
short alive;
