#include "validator.h"

#define CRITERIA_COUNT 11

// name exact:    -name  <filename>
// name contains: -name -<filename>
// group exact: -group root
// user exact: -user claudio
// perm min: -perm -000
// perm exact: -perm 000
// size: -size (+-)n(GMkc)
// format numerique: n, +n,-name

// DATE (ref NOW):
// access time : -atime (+-)<time>[smhd]
// modified time minutes/days: -mtime (+-)<time>[smhd]
// creation time minutes/days: -ctime (+-)<time>[smhd]

// operators:
// negation: -not
// and: -and (or absence of operator)
// or: -or
// expr: '\( ... \)'

bool validate_or(parser_t *exp) {
    (void)exp;
    return true;
}

bool validate_and(parser_t *exp) {
    (void)exp;
    return true;
}

bool validate_not(parser_t *exp) {
    (void)exp;
    return true;
}
bool validate_name(parser_t *exp) {
    (void)exp;
    return true;
}
bool validate_user(parser_t *exp) {
    (void)exp;
    return true;
}
bool validate_group(parser_t *exp) {
    (void)exp;
    return true;
}
bool validate_perm(parser_t *exp) {
    (void)exp;
    return true;
}
bool validate_size(parser_t *exp) {
    (void)exp;
    return true;
}
bool validate_atime(parser_t *exp) {
    (void)exp;
    return true;
}
bool validate_mtime(parser_t *exp) {
    (void)exp;
    return true;
}
bool validate_ctime(parser_t *exp) {
    (void)exp;
    return true;
}

typedef bool (*validate_fn_t)(parser_t *);
parser_crit_t parser_criteria[CRITERIA_COUNT] = {OR, AND, NOT, NAME, USER, GROUP, PERM, SIZE, ATIME, MTIME, CTIME};
validate_fn_t validators[CRITERIA_COUNT] = {&validate_or,    &validate_and,   &validate_not,  &validate_name,
                                            &validate_user,  &validate_group, &validate_perm, &validate_size,
                                            &validate_atime, &validate_mtime, &validate_ctime};

bool validator_validate(char *filename, parser_t *exp) {
    if (!exp)
        return true;
    (void)filename;
    (void)exp;
    return true;
}