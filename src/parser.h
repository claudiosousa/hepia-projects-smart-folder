#ifndef PARSER_H
#define PARSER_H

#include <stdbool.h>
#include <stdlib.h>

typedef enum { AND, OR, NAME, OWNER, GROUP, PERM, SIZE, ATIME, MTIME, CTIME } parser_token_t;

typedef enum { MAX, MIX, EXACT } parser_comp_t;

typedef struct parser_t {
    bool not;
    parser_token_t token;
    unsigned int value;
    char *char_value;
    parser_comp_t comp;

    struct parser_t *next;
} parser_t;

parser_t *parser_parse(char *expression[], size_t size);
void parser_free(parser_t *expression);

#endif
