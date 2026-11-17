#include "../includes/parser.h"
#include <string.h>
#include <stdlib.h>


int parse_request_line(char *line, t_request *request)
{
    char *token;
    char *tokens[3] = {NULL};

    if (!line || !request)
        return (-1);

    token = strtok(line, " \t");

    for (int i = 0; i < 3 && token != NULL; i++)
    {
        tokens[i] = token;
        token = strtok(NULL, " \t");
    }

    if (token != NULL || !tokens[0] || !tokens[1] || !tokens[2])
        return (-1);

    request->method = tokens[0];
    request->path = tokens[1];
    request->version = tokens[2];

    return (0);
}

void free_headers(t_header *headers)
{
    t_header *current;
    t_header *next;

    current = headers;

    while (current)
    {
        next = current->next;
        free(current);
        current = next;
    }
}

int parse_header(char *header, t_request *request)
{
    char *line;
    char *next_line;
    char *colon;
    char *name;
    char *value;
    t_header *new_header;
    t_header **tail;

    if (!header || !request)
        return (-1);

    line = header;
    tail = &request->headers;

    while (*line != '\r' && *line != '\0')
    {
        next_line = strstr(line, "\r\n");
        if (!next_line)
            return (-1);

        *next_line = '\0';

        colon = strchr(line, ':');
        if (!colon)
            return (-1);

        *colon = '\0';

        name = line;
        value = colon + 1;

        while (*value == ' ' || *value == '\t')
            value++;

        new_header = malloc(sizeof(t_header));
        if (!new_header)
            return (-1);

        new_header->name = name;
        new_header->value = value;
        new_header->next = NULL;

        *tail = new_header;
        tail = &new_header->next;

        line = next_line + 2;
    }

    return (0);
}


int parse_body(char *body, t_request *request)
{
    if (!body || !request)
        return (-1);

    request->body = body;

    return (0);
}


int parse_request(char *buff, t_request *request)
{
    char *line_end;
    char *headers_end;
    char *body;

    if (!buff || !request)
        return (-1);

    request->method = NULL;
    request->path = NULL;
    request->version = NULL;
    request->headers = NULL;
    request->body = NULL;

    line_end = strstr(buff, "\r\n");
    headers_end = strstr(buff, "\r\n\r\n");

    if (!line_end || !headers_end)
        return (-1);

    body = headers_end + 4;

    *line_end = '\0';

    if (parse_request_line(buff, request) < 0)
        return (-1);

    if (parse_header(line_end + 2, request) < 0)
    {
        free_headers(request->headers);
        request->headers = NULL;
        return (-1);
    }

    if (parse_body(body, request) < 0)
    {
        free_headers(request->headers);
        request->headers = NULL;
        return (-1);
    }

    return (0);
}


