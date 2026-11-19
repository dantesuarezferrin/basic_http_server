#include "../includes/parser.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

void handle_error(char *reply){
	strcpy(reply, "HTTP/1.1 404 Not Found\r\n\r\n");
}

int handle_root(char *reply){
	strcpy(reply, "HTTP/1.1 200 OK\r\n\r\n");
	return 1;
}

int handle_echo(t_request *request, char *reply){
	char *echo = strstr(request->path, "/echo/") + strlen("/echo/");
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
	return 1;
}

int handle_user_agent(t_request *request, char *reply){
	t_header *header = get_header("user-agent", request);
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
		return 1;
	}		
	strcpy(reply, "HTTP/1.1 400 BAD REQUEST\r\n\r\n");
	return 0;
}

int handle_file_get(t_request *request, char *reply, const char *arg_dir, int cfd){
	if( !arg_dir || strstr(request->path, "..")!=NULL ){
		strcpy(reply, "HTTP/1.1 404 Not Found\r\n\r\n");
		return 0;
	}
	else{
		char file_dir[1024];
		snprintf(file_dir, sizeof(file_dir), "%s/%s", arg_dir, request->path + 7);
		FILE *file = fopen(file_dir, "rb");
		long file_size = -1;
		if(file==NULL){
			strcpy(reply, "HTTP/1.1 404 Not Found\r\n\r\n");
			return 0;
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
				return -1;
			}
			else{
				rewind(file);
				size_t bytes_read = fread(file_content, 1, file_size, file);
				if( bytes_read != (size_t)file_size){
					perror("bytes_read");
					strcpy(reply, "HTTP/1.1 500 Internal Server Error\r\n\r\n");
					fclose(file);
					free(file_content);
					return -1;
				}else{

					snprintf(reply, 4096,
						"HTTP/1.1 200 OK\r\n"
						"Content-Type: application/octet-stream\r\n"
						"Content-Length: %ld\r\n"
						"Connection: close\r\n"
						"\r\n"
						, file_size
					);
								
					if(write(cfd, reply, strlen(reply)) < 0){
						perror("write");
						strcpy(reply, "HTTP/1.1 500 Internal Server Error\r\n\r\n");
					}
					if(write(cfd, file_content, file_size) < 0){
						perror("write file");
						strcpy(reply, "HTTP/1.1 500 Internal Server Error\r\n\r\n");
					}
					strcpy(reply, "");
				}
			}
			
			free(file_content);
			fclose(file);
		}
	}
	return 1;
}


int handle_file_post(t_request *request, char *reply, const char *arg_dir){
	
	if( arg_dir == NULL || strstr(arg_dir, "..") != 0 ){
		strcpy(reply, "HTTP/1.1 404 Not Found\r\n\r\n");
		return -1;
	}

	char file_dir[1024];
	snprintf(file_dir, sizeof(file_dir), "%s/%s", arg_dir, request->path + 7);
	FILE *file = fopen(file_dir, "wb");
	long file_size = -1;
	
	if(file==NULL){
		strcpy(reply, "HTTP/1.1 500 Internal Server Error\r\n\r\n");
		return -1;
	}

	file_size = strlen(request->body);
	if( fwrite(request->body, 1, file_size, file) != (size_t)file_size){
		strcpy(reply, "HTTP/1.1 500 Internal Server Error\r\n\r\n");
		fclose(file);
		return -1;
	}

	strcpy(reply, "HTTP/1.1 201 Created\r\n\r\n");

	fclose(file);
	return 1;
}

