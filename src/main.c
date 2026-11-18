#include "../includes/parser.h"
#include <sys/socket.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>

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
	char reply[4096]; 
	
	ssize_t req_size;
	char req_buff[4096];
	
	while (1) {
		peer_addr_size = sizeof(peer_addr);
		cfd = accept(sockfd, (struct sockaddr *)&peer_addr, (socklen_t *)&peer_addr_size);
		if(cfd < 0 ){
			perror("accept");
		}
		else{
			
			req_size = read(cfd, req_buff, 4095);
			
			if(req_size < 0){
				perror("read");
				close(cfd);
			}else{

				req_buff[req_size] = '\0';
				t_request request;
				
				if(parse_request(req_buff, &request) != 0){
					perror("parse request");
					close(cfd);
					continue;
				}


				if( strcmp(request.path, "/") == 0)
					strcpy(reply, "HTTP/1.1 200 OK\r\n\r\n");
				else if( strncmp(request.path, "/echo/", 6) == 0 ){
					char *echo = strstr(request.path, "/echo/")+strlen("/echo/");
					int echo_size = strlen(echo);
					snprintf(reply, 4096,
						"HTTP/1.1 200 OK\r\n"
						"Content-Type: text/plain\r\n"
						"Content-Length: %d\r\n"
						"Connection: close\r\n"
						"\r\n"
						"%s"
						, echo_size, echo
					);
				}
				else if( strcmp("/user-agent", request.path) == 0 ){
					
					t_header *header = get_header("user-agent", &request);
					if( header ){
						snprintf(reply, 4096,
							"HTTP/1.1 200 OK\r\n"
							"Content-Type: text/plain\r\n"
							"Content-Length: %zu\r\n"
							"Connection: close\r\n"
							"\r\n"
							"%s"
							, strlen(header->value), header->value
						);
					}
				}
				else if( strncmp("/files/", request.path, 7) == 0 ){

					if( !arg_dir || strstr("..", request.path)!=NULL ){
						strcpy(reply, "HTTP/1.1 404 Not Found\r\n\r\n");
					}
					else{

						char file_dir[1024];
						snprintf(file_dir, sizeof(file_dir), "%s/%s", arg_dir, request.path + 7);
						FILE *file = fopen(file_dir, "r");
						long file_size = -1;
						if(file==NULL){
							strcpy(reply, "HTTP/1.1 404 Not Found\r\n\r\n");
						}
						else{
							fseek(file , 0, SEEK_END);
							file_size = ftell(file);
							char *file_content=NULL;
							if (file_size >= 0 ) {
								file_content = (char *)malloc(file_size+1);
							}
							if( file_size < 0){
								strcpy(reply, "HTTP/1.1 404 Not Found\r\n\r\n");
							}
							else{
								rewind(file);
								size_t bytes_read = fread(file_content, 1, file_size, file) ;
								if( bytes_read != (size_t)file_size){
									strcpy(reply, "HTTP/1.1 404 Not Found\r\n\r\n");
								}
								else{
									file_content[file_size] = '\0';
									snprintf(reply, 4096,
										"HTTP/1.1 200 OK\r\n"
										"Content-Type: application/octec-stream\r\n"
										"Content-Length: %ld\r\n"
										"Connection: close\r\n"
										"\r\n"
										"%s"
										, file_size, file_content
									);
								}
							}

							free(file_content);
							fclose(file);
						}
					}


				}
				else{
					strcpy(reply, "HTTP/1.1 404 Not Found\r\n\r\n");
				}
				
				
				if(write(cfd, reply, strlen(reply)) < 0)
					perror("write");
				
				free_headers(request.headers);
				close(cfd);
			}
		}
	}


	close(sockfd);
	return 0;
}
