#include <stdio.h>
#include <string.h>
#include <time.h>
#include <sys/types.h>
#include "validator.h"

#define CRITERIA_COUNT 11
#define PERM_OPTIONS_COUNT 9

// permission bit to flag
typedef enum {
    R = 4,  // read
    W = 2,  // write
    X = 1   // execute
} validate_perm_type_t;

// permission bits to shift left
typedef enum {
    U = 6,  // user
    G = 3,  // group
    O = 0   // others
} validate_perm_owner_t;

int PERM_OPTIONS[PERM_OPTIONS_COUNT] = {R << U, W << U, X << U, R << G, W << G, X << G, R << O, W << O, X << O};
int PERM_FLAGS[PERM_OPTIONS_COUNT] = {S_IRUSR, S_IWUSR, S_IXUSR, S_IRGRP, S_IWGRP, S_IXGRP, S_IROTH, S_IWOTH, S_IXOTH};

bool validate_exp_token(char *filename, struct stat *filestat, parser_t *exp);

bool validate_or(char *filename, struct stat *filestat, parser_t *exp) {
    return validate_exp_token(filename, filestat, exp->next) ||
           validate_exp_token(filename, filestat, exp->next ? exp->next->next : NULL);
}

bool validate_and(char *filename, struct stat *filestat, parser_t *exp) {
    return validate_exp_token(filename, filestat, exp->next) &&
           validate_exp_token(filename, filestat, exp->next ? exp->next->next : NULL);
}

bool validate_not(char *filename, struct stat *filestat, parser_t *exp) {
    return !validate_exp_token(filename, filestat, exp->next);
}

bool validate_name(char *filename, struct stat *filestat, parser_t *exp) {
    (void)filestat;
    return exp->comp == EXACT ? strcmp(filename, (char *)exp->value) == 0
                              : strstr(filename, (char *)exp->value) != NULL;
}

bool validate_user(char *filename, struct stat *filestat, parser_t *exp) {
    (void)filename;
    return filestat->st_uid == *(uint *)exp->value;
}

bool validate_group(char *filename, struct stat *filestat, parser_t *exp) {
    (void)filename;
    return filestat->st_gid == *(uint *)exp->value;
}

bool validate_perm(char *filename, struct stat *filestat, parser_t *exp) {
    (void)filename;
    mode_t filemode = filestat->st_mode;
    int refperm = *(int *)exp->value;
    bool exact_perm = exp->comp == EXACT;
    for (int i = 0; i < PERM_OPTIONS_COUNT; i++) {
        bool req_perm = !!(refperm & PERM_OPTIONS[i]);
        bool file_perm = !!(filemode & PERM_FLAGS[i]);
        if (req_perm != file_perm && (exact_perm || !file_perm))
            return false;  // perm mismatch
    }
    return true;
}

bool validate_size(char *filename, struct stat *filestat, parser_t *exp) {
    (void)filename;
    long refsize = *(long *)exp->value;
    return exp->comp == EXACT ? refsize == filestat->st_size : (exp->comp == MIN) ^ !!(refsize < filestat->st_size);
}

bool validate_time(time_t comp_time, parser_comp_t comp, long refseconds) {
    time_t now = time(NULL);
    long s_delta = (long)difftime(now, comp_time);
    return comp == EXACT ? s_delta == refseconds : (comp == MIN) ^ !!(refseconds < s_delta);
}

bool validate_atime(char *filename, struct stat *filestat, parser_t *exp) {
    (void)filename;
    return validate_time(filestat->st_atime, exp->comp, *(long *)exp->value);
}

bool validate_mtime(char *filename, struct stat *filestat, parser_t *exp) {
    (void)filename;
    return validate_time(filestat->st_mtime, exp->comp, *(long *)exp->value);
}

bool validate_ctime(char *filename, struct stat *filestat, parser_t *exp) {
    (void)filename;
    return validate_time(filestat->st_ctime, exp->comp, *(long *)exp->value);
}

typedef bool (*validate_fn_t)(char *, struct stat *, parser_t *);
parser_crit_t parser_criteria[CRITERIA_COUNT] = {OR, AND, NOT, NAME, USER, GROUP, PERM, SIZE, ATIME, MTIME, CTIME};
validate_fn_t validators[CRITERIA_COUNT] = {&validate_or,    &validate_and,   &validate_not,  &validate_name,
                                            &validate_user,  &validate_group, &validate_perm, &validate_size,
                                            &validate_atime, &validate_mtime, &validate_ctime};

bool validate_exp_token(char *filename, struct stat *filestat, parser_t *exp) {
    if (!exp) {
        fprintf(stderr, "Expected expression but found NULL");
        return false;
    }

    return validators[exp->crit & 0b1111](filename, filestat, exp);
}

bool validator_validate(char *filename, struct stat *filestat, parser_t *exp) {
    return !exp || validate_exp_token(filename, filestat, exp);
}