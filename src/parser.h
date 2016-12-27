#ifndef PARSER_H
#define PARSER_H

#include <stdlib.h>
typedef enum {
    OPERATOR = 1 << 7,
    OPERATOR_1 = OPERATOR | 1 << 6,
    PARENTHESIS = 1 << 5,
    CRITERIA = 1 << 4,
} parser_crit_type_t;

// enum contains the criteria type encoded bitwise on enum values
typedef enum {
    AND = OPERATOR,  // operators below
    OR,
    NOT,
    LPARENTHESIS = PARENTHESIS,  // parenthesis below
    RPARENTHESIS,
    NAME = CRITERIA,  // all bellow are criteria
    USER,
    GROUP,
    PERM,
    SIZE,
    ATIME,
    MTIME,
    CTIME
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
