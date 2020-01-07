#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <time.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

#define BUFFLEN 1024
#define SERVER_PORT 8888
#define BACKLOG 5
#define PIDNUMB 2



void sig_fun(int signo)
{
	exit(-1);
}

void handle_connect(int s)
{
	struct sockaddr_in from;
	int len=sizeof(struct sockaddr_in);
	time_t now;
	char buff[BUFFLEN];
	int n=0;

	while(1)
	{
		memset(buff,0,BUFFLEN);
		n=recvfrom(s,buff,BUFFLEN,0,(struct sockaddr*)&from,&len);

		if(n>0&&!strncmp(buff,"TIME",4))
		{
			
			memset(buff,0,BUFFLEN);
			now=time(NULL);
			sprintf(buff,"%24s\r\n",ctime(&now));
			sendto(s,buff,strlen(buff),0,(struct sockaddr*)&from,len);

		}

	}

	close(s);

}

int main(int argc,char*argv[])
{
	int s_s;
	int res;
	struct sockaddr_in local;

	signal(SIGINT,sig_fun);

	s_s=socket(AF_INET,SOCK_DGRAM,0);

	if(s_s<0)
	{
		perror("socket() error!\n");
		return -1;
	}	

	memset(&local,0,sizeof(struct sockaddr_in));
	local.sin_family=AF_INET;
	local.sin_addr.s_addr=htonl(INADDR_ANY);
	local.sin_port=htons(SERVER_PORT);

	res=bind(s_s,(struct sockaddr*)&local,sizeof(local));

	if(res<0)
	{
		perror("listen() error!\n");
		return -1;
	}

	pid_t pid[PIDNUMB];

	for(int i=0;i<PIDNUMB;i++)
	{
		pid[i]=fork();
		if(pid[i]==0)
		{
			handle_connect(s_s);
		}

	}
	



}

