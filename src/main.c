#include <sys/socket.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include "../includes/connection.h"

#define PORT 4221
#define BACKLOG 64


int main(int argc, char *argv[]){
	if(argc < 1 || argc == 2 || argc > 3)
		return -1;

	char *arg_dir=NULL;
	
	if( argc != 1 ){
		if( strcmp("--directory", argv[1]) )
			return -1;
    arg_dir = argv[2];
	}

	int sockfd = socket(AF_INET, SOCK_STREAM, 0); 
	if (sockfd < 0) {
		perror("socker");
		return -1;
	}

	int opt = 1;
	if( setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR , &opt, sizeof(opt)) < 0){
		perror("setsockopt");
		close(sockfd);
		return -1;
	}

	struct sockaddr_in my_addr;
	memset(&my_addr, 0, sizeof(my_addr));
	my_addr.sin_family = AF_INET;
	my_addr.sin_port = htons(PORT);
	my_addr.sin_addr.s_addr = htonl(INADDR_ANY);

	socklen_t my_addrlen = sizeof(my_addr);
	
	if( bind(sockfd, (struct sockaddr *)&my_addr, my_addrlen) < 0){
		perror("bind");
		close(sockfd);
		return -1;
	}

	if( listen( sockfd, BACKLOG) < 0){
		perror("listen");
		close(sockfd);
		return -1;
	}

	struct sockaddr_in peer_addr;
	socklen_t peer_addr_size;
	int cfd;
	
	while (1) {
		peer_addr_size = sizeof(peer_addr);
		cfd = accept(sockfd, (struct sockaddr *)&peer_addr, (socklen_t *)&peer_addr_size);
		if(cfd < 0 ){
			perror("accept");
		}
		else{
			if( handle_connection(cfd, arg_dir) != 1 )
				printf("error en handle_client\n");
		}
	}


	close(sockfd);
	return 0;
}
