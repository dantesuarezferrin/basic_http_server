#ifndef HANDLERS_H
#define HANDLERS_H

#include "parser.h"

void handle_error(char *reply);
int handle_root(char *reply);
int handle_echo(t_request *request, char *reply);
int handle_user_agent(t_request *request, char *reply);
int handle_file_get(t_request *request, char *reply, const char *arg_dir, int cfd);
int handle_file_post(t_request *request, char *reply, const char *arg_dir);

#endif
