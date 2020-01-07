#include "overall.h"
#include "packet.h"
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
#include <pthread.h>
#include <sys/socket.h>
#include <signal.h>
int main(int argc,char**argv)
{
	struct hostent*host=NULL;
	struct protoent*protocol=NULL;
	char protoname[]="icmp";
	unsigned int inaddr=1;
	int size=128*K;

	if(argc<2)
	{
		icmp_usage();
		return -1;
	}
	protocol=getprotobyname(protoname);

	if(protocol==NULL)
	{
		perror("getprotobyname() error!\n");
		return -1;
	}

	memcpy(dest_str,argv[1],strlen(argv[1])+1);

	memset(pingpacket,0,sizeof(pingm_packet)*128);

	rawsock=socket(AF_INET,SOCK_RAW,protocol->p_proto);

	if(rawsock<0)
	{
		printf("error=%d\n",errno);
		perror("socket() error!\n");
		return -1;
	}

	pid=getuid();

	setsockopt(rawsock,SOL_SOCKET,SO_RCVBUF,&size,sizeof(size));
	bzero(&dest,sizeof(dest));

	dest.sin_family=AF_INET;

	inaddr=inet_addr(argv[1]);
	if(inaddr==INADDR_NONE)
	{
		host=gethostbyname(argv[1]);
		if(host==NULL)
		{
			perror("gethostbyname() error!\n");
			return -1;
		}
		memcpy((char*)&dest.sin_addr,host->h_addr,host->h_length);
	}
	else
	{
		memcpy((char*)&dest.sin_addr,&inaddr,sizeof(inaddr));
	}
	inaddr=dest.sin_addr.s_addr;

	printf("PING %s (%d.%d.%d.%d) 56(84) bytes of data.\n",dest_str,(inaddr&0x000000FF)>>0,(inaddr&0x0000FF00)>>8,(inaddr&0x00FF0000)>>16,(inaddr&0xFF000000)>>24);

	signal(SIGINT,icmp_sigint);

	alive=1;

	pthread_t send_id,recv_id;
	
	int err=0;

	err=pthread_create(&send_id,NULL,icmp_send,NULL);
	if(err<0)
	{
		perror("pthread() icmp_send error!\n");
		return -1;
	}	
	err=pthread_create(&recv_id,NULL,icmp_recv,NULL);
	if(err<0)
	{
		perror("pthread() icmp_recv error!\n");
		return -1;
	}

	pthread_join(send_id,NULL);
	pthread_join(recv_id,NULL);

	close(rawsock);	

	icmp_statistics();

	return 0;	

}
