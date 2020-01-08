#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <signal.h>
#include <netinet/in.h>
#include <sys/time.h>
#include <sys/ioctl.h>
#include <net/if.h>

#define IP_FOUND "IP_FOUND"
#define IP_FOUND_ACK "IP_FOUND_ACK"
#define BUFFER_LEN 32
#define MCAST_PORT 8889
#define IFNNAME "wlp3s0"


int main(void)
{
	int ret=-1;
	int so=-1;
	int so_broadcast=-1;
	struct ifreq ifr;
	struct sockaddr_in broadcast_addr;
	struct sockaddr_in from_addr;
	int from_len;
	int count=-1;
	fd_set readfd;
	char buff[BUFFER_LEN];
	struct timeval timeout;
	timeout.tv_sec=5;
	timeout.tv_usec=0;
	
	so=socket(AF_INET,SOCK_DGRAM,0);
	if(so<0)
	{
		perror("socket() error!\n");
		exit(-1);
	}

	strncpy(ifr.ifr_name,IFNNAME,strlen(IFNNAME));

	if(ioctl(so,SIOCGIFBRDADDR,&ifr)==-1)
	{
		perror("ioctl error!\n");
		exit(-1);
	}

	memcpy(&broadcast_addr,&ifr.ifr_broadaddr,sizeof(struct sockaddr_in));

	broadcast_addr.sin_port=htons(MCAST_PORT);

	ret=setsockopt(so,SOL_SOCKET,SO_BROADCAST,&so_broadcast,sizeof(so_broadcast));

	int times=10;
	int i=0;
	
	for(int i=0;i<times;i++)
	{
		ret=sendto(so,IP_FOUND,strlen(IP_FOUND),0,(struct sockaddr*)&broadcast_addr,sizeof(broadcast_addr));
		if(ret==-1)
		{
			continue;
		}
		FD_ZERO(&readfd);
		FD_SET(so,&readfd);			

		ret=select(so+1,&readfd,NULL,NULL,&timeout);
		switch(ret)
		{
			case -1:
				perror("select() error!\n");
				exit(-1);
			case 0:
				perror("time out!\n");
				exit(-1);
			default:
				if(FD_ISSET(so,&readfd))			
				{
					count=recvfrom(so,buff,BUFFER_LEN,0,(struct sockaddr*)&from_addr,&from_len);
					printf("Rec msg is %s\n",buff);
					if(strstr(buff,IP_FOUND_ACK))
					{
						printf("found server, IP is %s\n!",inet_ntoa(from_addr.sin_addr));
					}
					break;
				

				}
		

		}


	}
	return 0;
}
