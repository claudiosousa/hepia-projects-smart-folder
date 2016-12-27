#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "parser.h"

#define CRITERIA_COUNT 8
#define OPERATORS_COUNT 3
#define MIN_OP '-'
#define MAX_OP '+'

static parser_t *parse_base(char *argv, parser_crit_t criteria) {
    parser_t *res = malloc(sizeof(parser_t));
    res->next = NULL;
    res->crit = criteria;
    res->value = argv;

    char p_c = argv[0];
    res->comp = p_c == MIN_OP ? MIN : (p_c == MAX_OP ? MAX : EXACT);

    if (res->comp != EXACT)
        res->value = argv + 1;  // ignore first character

    return res;
}

static parser_t *parse_name(char *argv) {
    parser_t *res = parse_base(argv, NAME);

    if (res->comp == MAX)
        return NULL;
    return res;
}

static parser_t *parse_group(char *argv) {
    parser_t *res = parse_base(argv, GROUP);

    if (res->comp != EXACT)
        return NULL;
    return res;
}

static parser_t *parse_user(char *argv) {
    parser_t *res = parse_base(argv, USER);

    if (res->comp != EXACT)
        return NULL;
    return res;
}

static parser_t *parse_perm(char *argv) {
    parser_t *res = parse_base(argv, PERM);
    if (res->comp == MAX)
        return NULL;

    int val = strtol((char *)res->value, NULL, 8);

    if (val < 0 || val > 0777)  // invalid permission
        return NULL;
    res->value = malloc(sizeof(int));
    *(int *)res->value = val;
    return res;
}

/*
    @return the unity character. 0 if no unity char, -1 if error
*/
static char parse_exp_unity(char *exp) {
    int exp_len = strlen(exp);
    if (!exp_len)
        return -1;

    char last_char = exp[exp_len - 1];
    if (last_char <= '9')
        return 0;  // no unity char

    if (exp_len == 1)
        return -1;  // only unity without numeric value is invalid

    if (last_char <= 'Z')
        last_char += 'a' - 'A';  // tolower
    return last_char;
}

static parser_t *parse_size(char *argv) {
    long K_BINARY = powl(2, 10);

    parser_t *res = parse_base(argv, SIZE);

    char last_char = parse_exp_unity((char *)res->value);
    if (last_char == -1)
        return NULL;

    long unity_multiplier = 1;
    if (last_char != 0) {
        switch (last_char) {  // fallthrough switch
            case 'g':
                unity_multiplier *= K_BINARY;
            case 'm':
                unity_multiplier *= K_BINARY;
            case 'k':
                unity_multiplier *= K_BINARY;
            case 'c':
                break;
            default:
                return NULL;  // invalid unity character
        }
    }

    long val = atoi((char *)res->value);

    res->value = malloc(sizeof(long));
    *(long *)res->value = val * unity_multiplier;
    return res;
}

static parser_t *parse_time(char *argv, parser_crit_t criteria) {
    parser_t *res = parse_base(argv, criteria);

    char last_char = parse_exp_unity((char *)res->value);
    if (last_char == -1)
        return NULL;

    long unity_multiplier = 1;
    if (last_char != 0) {
        switch (last_char) {  // fallthrough switch
            case 'd':
                unity_multiplier *= 24;
            case 'h':
                unity_multiplier *= 60;
            case 'm':
                unity_multiplier *= 60;
            case 's':
                break;
            default:
                return NULL;  // invalid unity character
        }
    }

    long val = atoi((char *)res->value);

    res->value = malloc(sizeof(long));
    *(long *)res->value = val * unity_multiplier;

    return res;
}

static parser_t *parse_atime(char *argv) {
    return parse_time(argv, ATIME);
}

static parser_t *parse_ctime(char *argv) {
    return parse_time(argv, CTIME);
}

static parser_t *parse_mtime(char *argv) {
    return parse_time(argv, MTIME);
}

static parser_t *parse_op(parser_crit_t op) {
    parser_t *res = malloc(sizeof(parser_t));
    res->value = NULL;
    res->crit = op;
    return res;
}

typedef parser_t *(*parse_fn_t)(char *);

char *criteria[CRITERIA_COUNT] = {"-name", "-group", "-user", "-perm", "-size", "-atime", "-ctime", "-mtime"};
parse_fn_t criteria_type[CRITERIA_COUNT] = {&parse_name, &parse_group, &parse_user,  &parse_perm,
                                            &parse_size, &parse_atime, &parse_ctime, &parse_mtime};

char *operator[OPERATORS_COUNT] = {"-not", "-and", "-or"};
parser_crit_t operator_type[OPERATORS_COUNT] = {NOT, AND, OR};

parser_t *parse_token(char **expression[], size_t *size) {
    char *exp = *expression[0];
    *expression = *expression + 1;
    (*size)--;
    // TODO: handle parenthesis
    for (int i = 0; i < OPERATORS_COUNT; i++)  // an operator
        if (strcmp(exp, operator[i]) == 0)
            return parse_op(operator_type[i]);

    if (!*size)  // end of expression
        return NULL;

    char *exp_param = *expression[0];
    *expression = *expression + 1;
    (*size)--;
    for (int i = 0; i < CRITERIA_COUNT; i++)  // a criteria
        if (strcmp(exp, criteria[i]) == 0)
            return criteria_type[i](exp_param);

    return NULL;  // invalid token
}

parser_t *parser_parse(char *expression[], size_t size) {
    parser_t *res = NULL;
    while (size) {
        parser_t *parsed_token = parse_token(&expression, &size);
        if (parsed_token == NULL)
            return NULL;
        parsed_token->next = res;
        res = parsed_token;
    }

    return res;
}

void parser_free(parser_t *expression) {
    (void)expression;
}
