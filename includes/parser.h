#ifndef PARSER_H
#define PARSER_H


typedef struct s_header{
	char *name;
	char *value;
	struct s_header *next;
}t_header;

typedef struct s_request{
	char *method;
	char *path;
	char *version;
	t_header *headers;
	char *body;
}t_request;


int parse_request_line(char *line, t_request *request);
void free_headers(t_header *headers);
int parse_header(char *header, t_request *request);
int parse_body(char *body, t_request *request);
int parse_request(char *buff, t_request *request);


#endif
