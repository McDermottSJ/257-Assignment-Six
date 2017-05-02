#include<stdio.h>
#include<stdlib.h>
#include<arpa/inet.h>
#include<stdint.h>
#include<errno.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<sys/types.h>
#include<sys/stat.h>

int main(int argc, char *argv[]){
	int socket_fd;
	uint32_t value;
	struct sockaddr_in caddr;
	char *ip = "127.0.0.1";

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

	value = htonl(1);
	if(write(socket_fd, &value, sizeof(value)) != sizeof(value)){
		printf("error on writing network data [%s]\n", strerror(errno));
		return(-1);
	}
	printf("sent value of [%d]\n", ntohl(value));

	if(read(socket_fd, &value, sizeof(value)) != sizeof(value)){
		printf("error on reading network data [%s]\n", strerror(errno));
		return(-1);
	}
	value = ntohl(value);
	printf("Received a value of [%d]\n", value);

	close(socket_fd);
	return 0;
}
