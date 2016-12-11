#ifndef PARSER_H
#define PARSER_H

#include <stdlib.h>

typedef struct parser_t parser_t;

parser_t *parser_parse(char *expression[], size_t size);

#endif
