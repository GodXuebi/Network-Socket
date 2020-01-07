#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <time.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>


#define BUFFLEN 1024
#define SERVER_PORT 5000
#define BACKLOG 5
#define CLIENTNUM 2

pthread_mutex_t ALOCK=PTHREAD_MUTEX_INITIALIZER;

void*handle_request(void*argv)
{
	int s_s=*((int*)argv);
	int s_c;
	struct sockaddr_in from;
	int len=sizeof(from);

	while(1)
	{
		time_t now;
		char buff[BUFFLEN];
		int n=0;
	
		pthread_mutex_lock(&ALOCK);
		s_c=accept(s_s,(struct sockaddr*)&from,&len);
		pthread_mutex_unlock(&ALOCK);
		if(s_c<0)
		{
			perror("accpet() error!\n");
			return NULL;
		}
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


void handle_connect(int s_s)
{
	int s_c;
	struct sockaddr_in from;
	int len=sizeof(from);

	pthread_t thread_do[CLIENTNUM];
	for(int i=0;i<CLIENTNUM;i++)
	{
		pthread_create(&thread_do[i],NULL,handle_request,(void*)&s_s);
		
	}

	for(int i=0;i<CLIENTNUM;i++)
	{
		pthread_join(thread_do[i],NULL);
	}

}


int main(int argc,char*argv[])
{	
	int s_s;
	int res;
	struct sockaddr_in local;

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
		
	handle_connect(s_s);

	close(s_s);

	return 0;


}
