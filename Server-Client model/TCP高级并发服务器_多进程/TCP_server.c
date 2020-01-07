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
#define SERVER_PORT 10000
#define BACKLOG 5

int handle_request(int s_c)
{
	time_t now;
	char buff[BUFFLEN];
	int n=0;
	
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

int handle_connect(int s_s)
{
	int s_c;
	struct sockaddr_in from;
	int len=sizeof(from);

	while(1)
	{
		s_c=accept(s_s,(struct sockaddr*)&from,&len);
		if(s_c<0)
		{
			perror("accpet() error!\n");
			return -1;
		}
		if(s_c>0)
		{
			if(fork()>0)	
				close(s_c);
			else
			{
				handle_request(s_c);
				return 0;
			}

		}		
	}
	return 0;
}

int main(int argc,char*argv[])
{
	int s_s;
	int res;
	struct sockaddr_in local;

	s_s=socket(AF_INET,SOCK_STREAM,0);
	printf("first=%d\n",s_s);
	memset(&local,0,sizeof(struct sockaddr_in));
	local.sin_family=AF_INET;
	local.sin_addr.s_addr=htonl(INADDR_ANY);
	local.sin_port=htons(SERVER_PORT);

	bind(s_s,(struct sockaddr*)&local,sizeof(local));
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

	handle_connect(s_s);

	close(s_s);

	return 0;
	


}

