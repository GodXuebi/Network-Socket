#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <net/if_arp.h>
#include <string.h>
#include <unistd.h>
#include <sys/ioctl.h>

int main(int argc,char *argv[])
{
	int s;
	struct arpreq arpreq;
	struct sockaddr_in *addr=(struct sockaddr_in*)&arpreq.arp_pa;
	int err=-1;
	unsigned char *hw;
    	if(argc<2)
    	{
        	printf("argc error!\n");
        	return -1;
   	}
	s=socket(AF_INET,SOCK_DGRAM,0);
	if(s<0)
 	{
 		printf("socket()出错\n");
 		return -1;
 	}
	//填充arpreq的成员arp_pa
	addr->sin_family=AF_INET;
	addr->sin_addr.s_addr=inet_addr(argv[1]);
	if(addr->sin_addr.s_addr==INADDR_NONE)
	{
		printf("IP地址格式错误\n");
	}
	//网络接口为wlp3s0
	strcpy(arpreq.arp_dev,"wlp3s0");
	err=ioctl(s,SIOCGARP,&arpreq);

	if(err<0)
	{
		printf("IOCTL错误\n");
		return -1;
	}
	else
	{
		hw=(unsigned char*)&arpreq.arp_ha.sa_data;
		printf("%s:",argv[1]);
		printf("%02x:%02x:%02x:%02x:%02x:%02x",hw[0],hw[1],hw[2],hw[3],hw[4],hw[5]);
	}
	close(s);
	return 0;


}
