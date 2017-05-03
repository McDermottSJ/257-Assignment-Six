#include<stdio.h>
#include<stdlib.h>
#include<arpa/inet.h>
#include<errno.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<sys/types.h>
#include<sys/stat.h>

int setup(int *server, struct sockaddr_in *saddr);

int main(int argc, char *argv[]){
	int server, client, returnCode = 0;
	uint32_t value, inet_len;
	struct sockaddr_in saddr, caddr;
	char inString[9];
	char file[50] = "./";

	if(setup(&server, &saddr)){
		exit(-1);//print statement is already handled
	}
	while(1){
		
		inet_len = sizeof(caddr);
		//wait for a connection
		if((client = accept(server, (struct sockaddr *)&caddr, &inet_len)) == -1){
			printf("Error on client connection [%s]\n", strerror(errno));
			close(server);
			return(-1);	
		}
		printf("Server new client connection [%s/%d]\n", inet_ntoa(caddr.sin_addr), caddr.sin_port);

		//Wait for client request
		while(!returnCode){
			if(read(client, &inString, sizeof(inString)) != sizeof(inString)){
				printf("Error reading network data [%s]\n", strerror(errno));
				close(server);
				return(-1);	
			}
			printf("recieved packet [%s]\n", inString);//TODO remove
			
			//Check current directory for files
			strcat(file, inString);
			if(fopen(file, "r") == NULL){
				returnCode =0;
			}
			else{
				returnCode =1;
			}
	
			//Send response to client
			if(write(client, &returnCode, sizeof(returnCode)) != sizeof(returnCode)){
				printf("Error writing network data [%s]\n", strerror(errno));
				close(server);
				return(-1);	
			}
			
		}

		printf("located file [%s] successfully", inString);

	}

	return 0;
}



int setup(int *server, struct sockaddr_in *saddr){
	(*saddr).sin_family = AF_INET;
	(*saddr).sin_port = htons(2555);
	(*saddr).sin_addr.s_addr = htonl(INADDR_ANY);

	*server = socket (PF_INET, SOCK_STREAM, 0);
	if(*server == -1){
		printf("Error on socket creation [%s]\n", strerror(errno));
		return(-1);
	}

	if(setsockopt(*server, SOL_SOCKET, SO_REUSEADDR, &(int){1}, sizeof(int)) < 0){
		printf("socket keep open faild \n");
		return(-1);
	}
	
	if(bind(*server, (struct sockaddr *)saddr, sizeof(struct sockaddr)) == -1){
		printf("Error on socket bind [%s]\n", strerror(errno));
		return(-1);	
	}
	if(listen(*server, 5) == -1){
		printf("Error on socket listen [%s]\n", strerror(errno));
		return(-1);	
	}

	return 0;
}
