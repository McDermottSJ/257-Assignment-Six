#include<stdio.h>
#include<stdlib.h>
#include<arpa/inet.h>
#include<errno.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<sys/types.h>
#include<sys/stat.h>


int main(int argc, char *argv[]){
	int server, client;
	uint32_t value, inet_len;
	struct sockaddr_in saddr, caddr;

	saddr.sin_family = AF_INET;
	saddr.sin_port = htons(2555);
	saddr.sin_addr.s_addr = htonl(INADDR_ANY);

	server = socket (PF_INET, SOCK_STREAM, 0);
	if(server == -1){
		printf("Error on socket creation [%s]\n", strerror(errno));
		return(-1);
	}

	if(setsockopt(server, SOL_SOCKET, SO_REUSEADDR, &(int){1}, sizeof(int)) < 0){
		printf("socket keep open faild \n");
		return(-1);
	}
	
	if(bind(server, (struct sockaddr *)&saddr, sizeof(struct sockaddr)) == -1){
		printf("Error on socket bind [%s]\n", strerror(errno));
		return(-1);	
	}
	if(listen(server, 5) == -1){
		printf("Error on socket listen [%s]\n", strerror(errno));
		return(-1);	
	}
	while(1){
		
		inet_len = sizeof(caddr);
		if((client = accept(server, (struct sockaddr *)&caddr, &inet_len)) == -1){
			printf("Error on client connection [%s]\n", strerror(errno));
			close(server);
			return(-1);	
		}
		printf("Server new client connection [%s/%d]", inet_ntoa(caddr.sin_addr), caddr.sin_port);
		if(read(client, &value, sizeof(value)) != sizeof(value)){
			printf("Error reading network data [%s]\n", strerror(errno));
			close(server);
			return(-1);	
		}
		if(write(client, &value, sizeof(value)) != sizeof(value)){
			printf("Error writing network data [%s]\n", strerror(errno));
			close(server);
			return(-1);	
		}

		printf("Sent a value of [%d]\n", value);
	}

	return 0;
}
