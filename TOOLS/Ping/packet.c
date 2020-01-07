#include "packet.h"
#include "overall.h"
void icmp_pack(struct icmp*icmph,int seq,int length)
{
	unsigned char i=0;
	icmph->icmp_type=ICMP_ECHO;
	icmph->icmp_code=0;
	icmph->icmp_cksum=0;
	icmph->icmp_seq=seq;	
	icmph->icmp_id=pid&0xffff; //icmp_id contains two bytes while pid contains four bytes.
	for(int i=0;i<length;i++)
	{
		icmph->icmp_data[i]=i;//For each loop, the data content increases four bytes.
	}
	icmph->icmp_cksum=icmp_chksum((unsigned short*)icmph,length); //Here, we only calculate the cksum of 64 bytes because in other file, it is written that the PING only sends 64 bytes;
}


int icmp_unpack(char*buf,int len)
{
	int i,iphrlen;
	struct ip*ipdata = NULL;
	struct icmp*icmpdata=NULL;
	int rrt;
	
	ipdata=(struct ip*)buf;//Transfer the buf into the ip type;
	iphrlen=ipdata->ip_hl*4;//The length of header is counted by 4 bytes;
	icmpdata=(struct icmp*)(buf+iphrlen);
	len-=4;

    	if(len < 8)   //The ICMP at least contains 8 bytes for the header; 
    	{
        	fprintf(stderr, "Invalid icmp packet.Its length is less than 8\n");
        	return -1;
    	}

	//Now we want to judge if the ICMP_RECO is the reply for the ICMP_REQUEST sent by our program.
	if((icmpdata->icmp_type==ICMP_ECHOREPLY)&&(icmpdata->icmp_id==pid&0xffff)) 
	{
		struct timeval tv_interval,tv_recv,tv_send;
		pingm_packet*packet=icmp_findpacket(icmpdata->icmp_seq);
		if(packet==NULL)
		{
			perror("icmp_findpacket() error!\n");
			return -1;
		}
		packet->flag=0;

		tv_send=packet->tv_begin;

		gettimeofday(&tv_recv,NULL);

		tv_interval=icmp_tvsub(&tv_recv,&tv_send);

		rrt=tv_interval.tv_sec*1000+tv_interval.tv_usec/1000;
		printf("%d byte from %s:icmp_seq=%u ttl=%d, rrt=%d ms\n",len,inet_ntoa(ipdata->ip_src),icmpdata->icmp_seq,ipdata->ip_ttl,rrt);


		packet_recv++;	
		return len;
	}

	else
	{
		return -1;
	}

}


pingm_packet*icmp_findpacket(int seq)
{
	int i=0;
	pingm_packet*found=NULL;
	if(seq==-1)
	{
		for(int i=0;i<128;i++)
		{
			if(pingpacket[i].flag==0)
			{
				found=&pingpacket[i];
				break;
			}
		}

	}
	else if(seq>=0)
	{
		for(int i=0;i<128;i++)
		{
			if(pingpacket[i].seq==seq)
			{
				found=&pingpacket[i];
				break;
			}
		}

	}

	return found;
}

struct timeval icmp_tvsub(struct timeval*end,struct timeval*begin)
{
	struct timeval tv;
	tv.tv_sec=end->tv_sec-begin->tv_sec;
	tv.tv_usec=end->tv_usec-begin->tv_usec;

	if(tv.tv_usec<0)
	{
		tv.tv_sec--;
		tv.tv_usec+=1000000;

	}
	return tv;
	
}

unsigned short icmp_chksum(unsigned short *addr,int len)
{       int nleft=len;
        int sum=0;
        unsigned short *w=addr;
        unsigned short answer=0;

      
        while(nleft>1)
        {       
            sum+=*w++;
            nleft-=2;
        }

        if( nleft==1)
        {       
            *(unsigned char *)(&answer)=*(unsigned char *)w;
            sum+=answer;
        }
        sum=(sum>>16)+(sum&0xffff);
        sum+=(sum>>16);
        answer=~sum;
        return answer;
}


void*icmp_send(void*argv)
{
	gettimeofday(&tv_begin,NULL);
	while(alive)
	{
		int size=0;
		pingm_packet*packet=icmp_findpacket(-1);

		if(packet)
		{
			packet->seq=packet_send;
			packet->flag=1;
			gettimeofday(&(packet->tv_begin),NULL);	
		}

		icmp_pack((struct icmp*)send_buff,packet_send,64);

		size=sendto(rawsock,send_buff,64,0,(struct sockaddr*)&dest,sizeof(dest));

		if(size<0)
		{
			perror("send to() error!\n");
			continue;
		}
		packet_send++;
//		printf("send %d\n",packet_send);
		sleep(1);
	}	
}


void*icmp_recv(void*argv)
{
	struct timeval tv;
	tv.tv_usec=200;
	tv.tv_sec=0;
	
	fd_set readfd;

	while(alive)
	{
		int ret=0;
		FD_ZERO(&readfd);
		FD_SET(rawsock,&readfd);
		ret=select(rawsock+1,&readfd,NULL,NULL,&tv);
		int size=0;
		
		switch(ret)
		{
			case -1:
				perror("select() error()!\n");
				break;
			case 0:
				continue;
			default:
				size=recv(rawsock,recv_buff,sizeof(recv_buff),0);
				if(errno==EINTR)
				{
					perror("recv() error!\n");
					continue;

				}
				ret=icmp_unpack(recv_buff,size);
				if(ret==-1)
				{
					continue;

				}

		}

	}

}

void icmp_statistics(void)
{
	long time=(tv_interval.tv_sec*1000)+(tv_interval.tv_usec/1000);
	printf("---%s ping statistics ---\n",dest_str);
	printf("%d packets transmitted, %d received, %d%c packet loss, time %ld ms \n",packet_send,packet_recv,(packet_send-packet_recv)*100/packet_send,'%',time);
}

void icmp_usage(void)
{
	printf("ping aaa.bbb.ccc.ddd\n");
}

void icmp_sigint(int signo)
{
	alive=0;
	gettimeofday(&tv_end,NULL);
	tv_interval=icmp_tvsub(&tv_end,&tv_begin);
}
