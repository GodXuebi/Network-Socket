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
#define SERVER_PORT 5555
#define BACKLOG 5
#define PIDNUMB 3

void sig_fun(int signo)
{
	exit(1);
}


void handle_connect(int s_s)
{
	int s_c;
	struct sockaddr_in from;
	int len=sizeof(from);
	time_t now;
	char buff[BUFFLEN];
	int n=0;	


	while(1)
	{
		s_c=accept(s_s,(struct sockaddr*)&from,&len);
		if(s_c<0)
		{
			perror("accpet() error!\n");
			return ;
		}		
	
		memset(buff,0,BUFFLEN);
		n=recv(s_c,buff,BUFFLEN,0);

		if(n>0&&!strncmp(buff,"TIME",4))
		{
			
			memset(buff,0,BUFFLEN);
			now=time(NULL);
			sprintf(buff,"%24s\r\n",ctime(&now));
			send(s_c,buff,strlen(buff),0);
		}
		close(s_c);

	}

}


int main(int argc,char*argv[])
{
	int s_s;
	int res;
	struct sockaddr_in local;

	signal(SIGINT,sig_fun);

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

	pid_t pid[PIDNUMB];

	for(int i=0;i<PIDNUMB;i++)
	{
		pid[i]=fork();
		if(pid[i]==0)
		{
			handle_connect(s_s);
		}

	}

	while(1);

	close(s_s);	
}
