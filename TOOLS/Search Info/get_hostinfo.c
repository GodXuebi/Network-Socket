#include <netdb.h>
#include <string.h>
#include <stdio.h>
#include <arpa/inet.h>
int main(int argc,char*argv[])
{
	
	char host[40];
	strcpy(host,argv[1]);
	struct hostent*ht=NULL;
	char str[30];
	ht=gethostbyname(host);

	if(ht)
	{
		int i=0;
		printf("get the host name: %s\n",host);
		printf("protocol type:%s\n",ht->h_addrtype==AF_INET?"AF_INET":"AF_INET6");
		printf("length:%d\n",ht->h_length);
		for(i=0;;i++)
		{
			if(ht->h_addr_list[i]!=NULL)
			{
				printf("IP%d:%s\n",i,inet_ntop(ht->h_addrtype,ht->h_addr_list[i],str,30));
			}
			else
			{
				break;
			}
		}
		for(i=0;;i++)
		{
			if(ht->h_aliases[i]!=NULL)
			{
				printf("aliases%d:%s\n",i,ht->h_aliases[i]);
			}
			else
			{
				break;
			}
		}
	}
	return 0;
}
