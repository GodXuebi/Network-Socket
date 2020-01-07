#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <time.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>

#define BUFFLEN 1024
#define SERVER_PORT 8888
#define BACKLOG 5

int main(int argc,char*argv[])
{
	int s_s,s_c;
	struct sockaddr_in local,from;
	time_t now;
	char buff[BUFFLEN];
	int n=0;
	int len=sizeof(from);
	int res;

	s_s=socket(AF_INET,SOCK_STREAM,0);

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
		perror("bind() error!\n");
		return -1;
	}

	res=listen(s_s,BACKLOG);
	if(res<0)
	{
		perror("listen() error!\n");
		return -1;
	}

	while(1)
	{
		s_c=accept(s_s,(struct sockaddr*)&from,&len);
		if(s_c<0)
		{
			perror("accpet() error!\n");
			return -1;
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
	close(s_s);

	return 0;


}
