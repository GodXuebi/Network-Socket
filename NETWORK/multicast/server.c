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

#define IP_FOUND "IP_FOUND"
#define IP_FOUND_ACK "IP_FOUND_ACK"
#define BUFFER_LEN 32
#define MCAST_PORT 8889
int main(void)
{
	int ret=-1;
	int so=-1;
	struct sockaddr_in local_addr,from_addr;
	int from_len;
	int count=-1;
	fd_set readfd;
	char buff[BUFFER_LEN];
	struct timeval timeout;
	timeout.tv_sec=5;
	timeout.tv_usec=0;
	
	printf("set basic information!\n");

	so=socket(AF_INET,SOCK_DGRAM,0);
	if(so<0)
	{
		perror("socket() error!\n");
		exit(-1);
	}

	memset(&local_addr,0,sizeof(struct sockaddr_in));


	local_addr.sin_family=AF_INET;
	local_addr.sin_port=htons(MCAST_PORT);
	local_addr.sin_addr.s_addr=htonl(INADDR_ANY);

	ret=bind(so,(struct sockaddr*)&local_addr,sizeof(local_addr));

	if(ret==-1)
	{
		perror("bind() error!\n");
		exit(-1);
	}

	while(1)
	{
		FD_ZERO(&readfd);
		FD_SET(so,&readfd);

		ret=select(so+1,&readfd,NULL,NULL,&timeout);
		switch(ret)
		{
			case -1:
				perror("select() error!\n");
				exit(-1);
			case 0:
				printf("time out!\n");
				exit(-1);
			default :
				if(FD_ISSET(so,&readfd))
				{
					count=recvfrom(so,buff,BUFFER_LEN,0,(struct sockaddr*)&from_addr,&from_len);
//					printf("count=%d\n",count);
					buff[count]='\0';
					printf("recv message is %s\n",buff);
					sleep(0.1);
					if(strstr(buff,IP_FOUND))
					{
						printf("contain IP_FOUND\n");
						memcpy(buff,IP_FOUND_ACK,strlen(IP_FOUND_ACK)+1);
						count=sendto(so,buff,strlen(buff),0,(struct sockaddr*)&from_addr,from_len);
					}

				}
		}
		printf("IP_FOUND!\n");
		return 0;
	}

	

	
	


}
