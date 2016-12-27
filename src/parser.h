#ifndef PARSER_H
#define PARSER_H

#include <stdlib.h>

typedef enum { AND, OR, NOT, NAME, USER, GROUP, PERM, SIZE, ATIME, MTIME, CTIME } parser_crit_t;

typedef enum { MAX, MIN, EXACT } parser_comp_t;

typedef struct parser_t {
    parser_crit_t crit;
    void * value;
    parser_comp_t comp;

    struct parser_t *next;
} parser_t;

parser_t *parser_parse(char *expression[], size_t size);
void parser_free(parser_t *expression);

#endif
