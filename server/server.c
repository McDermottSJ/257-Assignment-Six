#include<stdio.h>
#include<stdlib.h>
#include<arpa/inet.h>
#include<errno.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<signal.h>

int killed =0;

int setup(int *server, struct sockaddr_in *saddr);

void signalHandler(int no){
	printf("\n caught interupt signal, finishing up.\n");
	killed = 1;
	return;
}

int main(int argc, char *argv[]){
	int server, client, returnCode = 0, fileEnd = 0, i;
	uint32_t value, inet_len;
	struct sockaddr_in saddr, caddr;
	char inString[50];
	char filePath[50] = "./";
	FILE *file;
	char fileBuffer[50];
	char endOfPackets[50] = {'c','m','s','c','2','5','7',[7 ... 49] = '\0' };

	signal(SIGINT, signalHandler);

	if(setup(&server, &saddr)){
		exit(-1);//print statement is already handled
	}
	while(!killed){
			inet_len = sizeof(caddr);
			//wait for a connection
			if((client = accept(server, (struct sockaddr *)&caddr, &inet_len)) == -1){
				printf("Error on client connection [%s]\n", strerror(errno));
				close(server);
				return(-1);	
			}
			printf("Server new client connection [%s/%d]\n", inet_ntoa(caddr.sin_addr), caddr.sin_port);

			//Wait for client request
			if(read(client, &inString, sizeof(inString)) != sizeof(inString)){
				printf("Error reading network data [%s]\n", strerror(errno));
				close(server);
				return(-1);	
			}
			printf("recieved packet [%s]\n", inString);//TODO remove
			//reset file path
			for(i = 2; i < 50; i++){
				filePath[i] = '\0';
			}
			//Check current directory for files
			strcat(filePath, inString);
			if((file = fopen(filePath, "r")) == NULL){
				returnCode =0;
				//file not found
			}
			else{
				returnCode =1;
				//file found
			}
	
			//Send response to client
			if(write(client, &returnCode, sizeof(returnCode)) != sizeof(returnCode)){
				printf("Error writing network data [%s]\n", strerror(errno));
				close(server);
				return(-1);	
			}

			if(returnCode == 0){
				continue;
			}
			

		printf("located file [%s] successfully", inString);
		
		//begin sending file
		int i = 0;
		char c;
		
		fileEnd = 0;
		while(!fileEnd){
			//grab 50 bytes from file, pad packet if needed.
			for(i = 0; i < 50; i++){
				c = fgetc(file);
				if(feof(file)){
					fileBuffer[i] = '\0';
					fileEnd = 1;
				}
				else{
					fileBuffer[i] = c;
				}
			}
			//send packet
			if(write(client, &fileBuffer, sizeof(fileBuffer)) != sizeof(fileBuffer)){
				printf("Error writing network data [%s]\n", strerror(errno));
				close(server);
				return(-1);	
			}
			//wait for response
			if(read(client, &returnCode, sizeof(returnCode)) != sizeof(returnCode)){
				printf("Error reading network data [%s]\n", strerror(errno));
				close(server);
				return(-1);	
			}
		}
		//send end of packets signal
		if(write(client, &endOfPackets, sizeof(endOfPackets)) != sizeof(endOfPackets)){
			printf("Error writing network data [%s]\n", strerror(errno));
			close(server);
			return(-1);	
		}
		fclose(file);

	}
	//TODO RESUMER HERE-- add threading, remember visual mode and > to indent
	return 0;
}


int setup(int *server, struct sockaddr_in *saddr){
	(*saddr).sin_family = AF_INET;
	(*saddr).sin_port = htons(2666);
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
