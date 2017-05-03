#include<stdio.h>
#include<stdlib.h>
#include<arpa/inet.h>
#include<stdint.h>
#include<errno.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<string.h>

int main(int argc, char *argv[]){
	int socket_fd, i;
	uint32_t value;
	struct sockaddr_in caddr;
	char *ip = "127.0.0.1";
	char buffedName[9] = "test.txt";
	char fileBuffer[50];
	FILE *file;
	int returnCode = 1;
	char endOfPackets[50] = {'c','m','s','c','2','5','7',[7 ... 49] = '\0' };
	//TODO RESUME HERE-- fix the input to take any file name

	/*
	if(argc > 1){
		scanf("%s", &file);
	}
	else{
		file = argv[1];
	}
	for(i = 0; i < 50; i++){
		if(i < strlen(file)){
			buffedFile[i] = file[i];
		}
		else{
			buffedFile[i] = '\0';
		}
	}

	printf("%s", buffedFile);
	*/


	//Setup
	caddr.sin_family = AF_INET;
	caddr.sin_port = htons(2555);
	if(inet_aton(ip, &caddr.sin_addr) == 0){
		return ( -1 );
	}

	socket_fd = socket(PF_INET, SOCK_STREAM, 0);
	if(socket_fd == -1){
		printf("error on socket creation [%s]\n", strerror(errno));
		return(-1);
	}

	if(connect(socket_fd, (const struct sockaddr *)&caddr, sizeof(struct sockaddr)) == -1){
		printf("error on socket connection [%s]\n", strerror(errno));
		return(-1);
	}


	//Exchange
	value = htonl(1);
	
	//send file name to server
	if(write(socket_fd, &buffedName, sizeof(buffedName)) != sizeof(buffedName)){
		printf("error on writing network data [%s]\n", strerror(errno));
		return(-1);
	}
	printf("sent packet\n");
	
	//wait for response
	if(read(socket_fd, &returnCode, sizeof(returnCode)) != sizeof(returnCode)){
		printf("error on reading network data [%s]\n", strerror(errno));
		return(-1);
	}
	
	
	printf("%d\n", returnCode);
	if(returnCode != 1){
		//TODO prompt for new file?
		return(-2);
	}
	else{
		file = fopen(buffedName, "w");
		
		//accept data stream
		while(1){
			if(read(socket_fd, &fileBuffer, sizeof(fileBuffer)) != sizeof(fileBuffer)){
				printf("error on reading network data [%s]\n", strerror(errno));
				return(-1);
			}
			//check for end of stream
			if(strcmp(fileBuffer, endOfPackets) == 0){
				fclose(file);
				break;
			}

			//write packet to file
			for(i =0; i < 50; i++){
				if(fileBuffer[i] != '\0'){
					fprintf(file, "%c", fileBuffer[i]);
				}else{
					break;
				}
			}
			//send finished reading packet signal
			returnCode = 1;
			if(write(socket_fd, &returnCode, sizeof(returnCode)) != sizeof(returnCode)){
				printf("error on writing network data [%s]\n", strerror(errno));
				return(-1);
			}
			
		}
	}
	
	close(socket_fd);
	
	return 0;
}
