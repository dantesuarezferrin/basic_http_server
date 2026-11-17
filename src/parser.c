#include "parser.h"
#include <string.h>
#include <stdlib.h>

#define LIMITERS " \t\r\n"
#define INITIAL_CAPACITY 64

char **parseLine(char *line){
	if(line == NULL)
		return NULL;

	char **params = malloc(sizeof(char *)* INITIAL_CAPACITY);
	
	if(params == NULL)
		return NULL;

	size_t n_params = 0;
	size_t size = INITIAL_CAPACITY;
	char *param; 
	char **tmp;

	param = strtok(line, LIMITERS);
	
	while(param != NULL ){
		if(n_params >= size -1){
			size*=2;
			tmp =realloc(params, size*sizeof(char *) );
			if(tmp == NULL){
				free(params);
				return NULL;
			}
			params = tmp;
		}
		params[n_params++] = param;
		param = strtok(NULL, LIMITERS);
	}
	params[n_params] = NULL;
	return params;
}
