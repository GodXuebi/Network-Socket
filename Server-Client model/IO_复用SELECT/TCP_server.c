#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <time.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/select.h>

#define BUFFLEN 1024
#define SERVER_PORT 2222
#define BACKLOG 5
#define CLIENTNUM 1024

int connect_host[CLIENTNUM];
int connect_number=0;

void*handle_request(void*argv)
{
		time_t now;
		char buff[BUFFLEN];
		int n=0;
		int maxfd=-1;
		fd_set scanfd;
		struct timeval timeout;
		timeout.tv_sec=1;
		timeout.tv_usec=0;

		int err=-1;

		while(1)
		{
			maxfd=-1;
			FD_ZERO(&scanfd);
			for(int i=0;i<CLIENTNUM;i++)
			{
				if(connect_host[i]!=-1)
				{
					FD_SET(connect_host[i],&scanfd);
					if(maxfd<connect_host[i])
						maxfd=connect_host[i];
				}
			}

		
	
			err=select(maxfd+1,&scanfd,NULL,NULL,&timeout);

			switch(err)
			{
				case 0:
					break;
				case -1:
					break;
				default:
					if(connect_number<0)
						break;
					for(int i=0;i<CLIENTNUM;i++)
					{
						if(connect_host[i]!=-1)
						{
							if(FD_ISSET(connect_host[i],&scanfd))
							{
								memset(buff,0,BUFFLEN);
								n=recv(connect_host[i],buff,BUFFLEN,0);
								if(n>0&&!strncmp(buff,"TIME",4))
								{
				
									memset(buff,0,BUFFLEN);
									now=time(NULL);
									sprintf(buff,"%24s\r\n",ctime(&now));
									send(connect_host[i],buff,strlen(buff),0);
								}
								connect_host[i]=-1;
								connect_number--;
								close(connect_host[i]);
							}
						}
			}
		}

		}
		return NULL;
}

void*handle_connect(void*argv)
{
	int s_s=*((int*)argv);
	struct sockaddr_in from;
	int len=sizeof(from);

	while(1)
	{
		int s_c=accept(s_s,(struct sockaddr*)&from,&len);
		unsigned int inaddr=from.sin_addr.s_addr;
		printf("a cilent connect, from (%d.%d.%d.%d)\n",(inaddr&0x000000FF)>>0,(inaddr&0x0000FF00)>>8,(inaddr&0x00FF0000)>>16,(inaddr&0xFF000000)>>24);
		for(int i=0;i<CLIENTNUM;i++)
		{
			if(connect_host[i]==-1)
			{
				connect_host[i]=s_c;
				connect_number++;
				break;
			}
		}
	}
	return NULL;

}

int main(int argc,char*argv[])
{
	int s_s;
	struct sockaddr_in local;
	int res;
	
	for(int i=0;i<CLIENTNUM;i++)
	{
		connect_host[i]=-1;
	}

	s_s=socket(AF_INET,SOCK_STREAM,0);

	memset(&local,0,sizeof(struct sockaddr_in));	
	local.sin_family=AF_INET;
	local.sin_addr.s_addr=htonl(INADDR_ANY);
	local.sin_port=htons(SERVER_PORT);

	res=bind(s_s,(struct sockaddr*)&local,sizeof(local));
	if(res<0)
	{
		perror("bind() error!\n");
		return -1;
	}
	
	res=listen(s_s,BACKLOG);
	if(res<0)
	{
		perror("listen() error!\n");
		return -1;
	}

	pthread_t thread_do[2];

	//创建线程，负责处理客户端连接
	pthread_create(&thread_do[0],NULL,handle_connect,(void*)&s_s);
	//创建线程，负责信息处理
	pthread_create(&thread_do[1],NULL,handle_request,NULL);

	for(int i=0;i<2;i++)
	{
		pthread_join(thread_do[i],NULL);
	}

	close(s_s);

	return 0;

}
