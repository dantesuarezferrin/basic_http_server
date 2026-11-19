#include "../includes/parser.h"
#include "../includes/handlers.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#define STD_BUFF 4096

static int handle_case(t_request *request){
	if( strcmp(request->method, "GET") == 0 ){
		
		if( strcmp(request->path, "/") == 0)
			return 0;
		
		if( strcmp(request->path, "/user-agent") == 0)
			return 1;
		
		if( strncmp(request->path, "/echo/", 6) == 0)
			return 2;
		
		if( strncmp(request->path, "/files/", 7) == 0)
			return 3;
		
		return -1;
	}
	
	if( strcmp(request->method, "POST") == 0 ){
		
		if( strncmp(request->path, "/files/", 7) == 0)
			return 4;
		
		return -1;
	}
	return -1;
}

int handle_connection(int cfd, const char *server_dir){
	ssize_t req_size;
	char req_buff[STD_BUFF];
	t_request request;
	char reply[STD_BUFF];
	reply[0] = '\0';

	req_size = read(cfd, req_buff, STD_BUFF-1);
	
	if( req_size < 0 ){
		perror("read");
		close(cfd);
		return -1;
	}

	req_buff[req_size] = '\0';

	if( parse_request(req_buff, &request) != 0 ){
		perror("parse_request");
		close(cfd);
		return -1;	
	}
	
	switch( handle_case(&request) ){
		case -1:
			handle_error(reply);
			break;
		case 0:
			handle_root(reply);
			break;
		case 1:
			handle_user_agent(&request, reply);
			break;
		case 2:
			handle_echo(&request, reply);
			break;
		case 3:
			handle_file_get(&request, reply, server_dir, cfd);
			break;
		case 4:
			handle_file_post(&request, reply, server_dir);
			break;

	}
	reply[strlen(reply)] = '\0';

	if( write( cfd, reply, strlen(reply)) < 0 ){
		perror("write");
		free_headers(request.headers);
		close(cfd);
		return -1;
	}
	free_headers(request.headers);
	close(cfd);
	return 1;
}
