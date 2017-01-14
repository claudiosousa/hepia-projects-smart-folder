#ifndef PARSER_H
#define PARSER_H

#include <stdlib.h>
typedef enum {
    OPERATOR = 1 << 7,
    PARENTHESIS = 1 << 6,
    CRITERIA = 1 << 5,
} parser_crit_type_t;

// enum contains the criteria type encoded bitwise on 3 most significant bits enum values
typedef enum {
    OR = OPERATOR,  // operators below, order by priority DESC
    AND,
    NOT,
    NAME = CRITERIA | 3,  // below are criteria
    USER,
    GROUP,
    PERM,
    SIZE,
    ATIME,
    MTIME,
    CTIME,
    LPARENTHESIS = PARENTHESIS,  // parenthesis below
    RPARENTHESIS,

} parser_crit_t;

typedef enum { MAX, MIN, EXACT } parser_comp_t;

typedef struct parser_t {
    parser_crit_t crit;
    void *value;
    parser_comp_t comp;

    struct parser_t *next;
} parser_t;

parser_t *parser_parse(char *expression[], size_t size);
void parser_free(parser_t *expression);

#endif
