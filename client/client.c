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
	char buffedFile[9] = "test.txt";
	char *file;
	int returnCode = 1;
	
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
	if(write(socket_fd, &buffedFile, sizeof(buffedFile)) != sizeof(buffedFile)){
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
	if(returnCode != 0){
		//TODO prompt for new file?
		return(-2);
	}
	else{
		//accept data stream
		while(1){
			
		}
	}

	close(socket_fd);
	
	return 0;
}
