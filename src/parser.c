#include <string.h>
#include "parser.h"

#define CRITERIA_COUNT 8
#define OPERATORS_COUNT 3

static parser_t *parse_name(char *argv) {
    (void)argv;
    return NULL;
}

static parser_t *parse_group(char *argv) {
    (void)argv;
    return NULL;
}

static parser_t *parse_user(char *argv) {
    (void)argv;
    return NULL;
}

static parser_t *parse_perm(char *argv) {
    (void)argv;
    return NULL;
}

static parser_t *parse_size(char *argv) {
    (void)argv;
    return NULL;
}

static parser_t *parse_atime(char *argv) {
    (void)argv;
    return NULL;
}

static parser_t *parse_ctime(char *argv) {
    (void)argv;
    return NULL;
}

static parser_t *parse_mtime(char *argv) {
    (void)argv;
    return NULL;
}

typedef parser_t *(*parse_fn_t)(char *);

char *criteria[CRITERIA_COUNT] = {"-name", "-group", "-user", "-perm", "-size", "-atime", "-ctime", "-mtime"};
parse_fn_t criteria_parse[CRITERIA_COUNT] = {&parse_name, &parse_group, &parse_user,  &parse_perm,
                                             &parse_size, &parse_atime, &parse_ctime, &parse_mtime};

char *operator[OPERATORS_COUNT] = {"-not", "-and", "-or"};
parser_crit_t operator_type[OPERATORS_COUNT] = {NOT, AND, NOT};

parser_t *parser_parse(char *expression[], size_t size) {
    (void)expression;
    (void)size;
    parser_t *res = NULL;
    // size_t exp_i = 0;
    // char *exp;

    // while (expression) {
    //     parser_t *parse_token = parse_token(&(expression[i]));
    //     exp = expression[exp_i];
    //     for (int i = 0; i < OPERATORS_COUNT)
    //         if (strcmp(exp, ))
    //             parse_token if () malloc(sizeof(parser_t));

    //     exp_i++;
    // }

    return res;
}

void parser_free(parser_t *expression) {
    (void)expression;
}
