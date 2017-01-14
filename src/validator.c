/** Critera and operator evaluation functions.
    @file

    Contains the logic to evaluate each possible critera and operator.
 */
#include <string.h>
#include <time.h>
#include <sys/types.h>
#include "validator.h"
#include "logger.h"

/** Number of criteria/operators */
#define CRITERIA_COUNT 11
/** Number of permission options */
#define PERM_OPTIONS_COUNT 9
/** Mask that to apply to `parser_crit_t` to get criteria index in the enum
    @see parser_crit_t */
#define CRITERIA_ORDER_MASK 0b1111

/** List of permission types.
          - read
          - write
          - execute

    @see validate_perm_owner_t
*/
typedef enum {
    R = 4,  // read
    W = 2,  // write
    X = 1   // execute
} validate_perm_type_t;

/** List of permission targets.
          - user
          - group
          - owner

    Used to shift `validate_perm_type_t` bitwise
    @see validate_perm_type_t
*/
typedef enum {
    U = 6,  // user
    G = 3,  // group
    O = 0   // others
} validate_perm_owner_t;

/** List of all permission options.

    Composed by combining `validate_perm_type_t` and `validate_perm_owner_t`
    @see validate_perm_type_t
    @see validate_perm_owner_t
 */
static int PERM_OPTIONS[PERM_OPTIONS_COUNT] = {R << U, W << U, X << U, R << G, W << G, X << G, R << O, W << O, X << O};

/** List of all [OS permission flags](http://pubs.opengroup.org/onlinepubs/7908799/xsh/sysstat.h.html) matching the
   permission options.

    @see validate_perm_type_t
    @see PERM_OPTIONS
 */
static int PERM_FLAGS[PERM_OPTIONS_COUNT] = {S_IRUSR, S_IWUSR, S_IXUSR, S_IRGRP, S_IWGRP,
                                             S_IXGRP, S_IROTH, S_IWOTH, S_IXOTH};

static bool validate_exp_token(char *filename, struct stat *filestat, parser_t *exp);

/** Validates OR operators. */
static bool validate_or(char *filename, struct stat *filestat, parser_t *exp) {
    return validate_exp_token(filename, filestat, exp->next) ||
           validate_exp_token(filename, filestat, exp->next ? exp->next->next : NULL);
}

/** Validates AND operators. */
static bool validate_and(char *filename, struct stat *filestat, parser_t *exp) {
    return validate_exp_token(filename, filestat, exp->next) &&
           validate_exp_token(filename, filestat, exp->next ? exp->next->next : NULL);
}

/** Validates NOT operators. */
static bool validate_not(char *filename, struct stat *filestat, parser_t *exp) {
    return !validate_exp_token(filename, filestat, exp->next);
}

/** Validates NAME criteria. */
static bool validate_name(char *filename, struct stat *filestat, parser_t *exp) {
    (void)filestat;
    return exp->comp == EXACT ? strcmp(filename, (char *)exp->value) == 0
                              : strstr(filename, (char *)exp->value) != NULL;
}

/** Validates USER criteria. */
static bool validate_user(char *filename, struct stat *filestat, parser_t *exp) {
    (void)filename;
    return filestat->st_uid == *(uint *)exp->value;
}

/** Validates GROUP criteria. */
static bool validate_group(char *filename, struct stat *filestat, parser_t *exp) {
    (void)filename;
    return filestat->st_gid == *(uint *)exp->value;
}

/** Validates PERM criteria. */
static bool validate_perm(char *filename, struct stat *filestat, parser_t *exp) {
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

/** Validates SIZE criteria. */
static bool validate_size(char *filename, struct stat *filestat, parser_t *exp) {
    (void)filename;
    long refsize = *(long *)exp->value;
    return exp->comp == EXACT ? refsize == filestat->st_size : (exp->comp == MIN) ^ !!(refsize < filestat->st_size);
}

/** Validates TIME criteria. */
static bool validate_time(time_t comp_time, parser_comp_t comp, long refseconds) {
    time_t now = time(NULL);
    long s_delta = (long)difftime(now, comp_time);
    return comp == EXACT ? s_delta == refseconds : (comp == MIN) ^ !!(refseconds < s_delta);
}

/** Validates ATIME criteria. */
static bool validate_atime(char *filename, struct stat *filestat, parser_t *exp) {
    (void)filename;
    return validate_time(filestat->st_atime, exp->comp, *(long *)exp->value);
}

/** Validates MTIME criteria. */
static bool validate_mtime(char *filename, struct stat *filestat, parser_t *exp) {
    (void)filename;
    return validate_time(filestat->st_mtime, exp->comp, *(long *)exp->value);
}

/** Validates CTIME criteria. */
static bool validate_ctime(char *filename, struct stat *filestat, parser_t *exp) {
    (void)filename;
    return validate_time(filestat->st_ctime, exp->comp, *(long *)exp->value);
}

/** Function pointer for criteria validate functions*/
typedef bool (*validate_fn_t)(char *, struct stat *, parser_t *);

/** List of  criteria validate functions.

    The order is important as it must match the index of the criteria in `parser_crit_t`

    @see parser_crit_t
    @see validate_fn_t
    @see CRITERIA_ORDER_MASK
*/
static validate_fn_t validators[CRITERIA_COUNT] = {&validate_or,    &validate_and,   &validate_not,  &validate_name,
                                                   &validate_user,  &validate_group, &validate_perm, &validate_size,
                                                   &validate_atime, &validate_mtime, &validate_ctime};

/** Validates an expression token.

    Retrieves the validate function corresponding to the expression token,
    which can be a criteria or an operator
*/
static bool validate_exp_token(char *filename, struct stat *filestat, parser_t *exp) {
    if (!exp) {
        logger_error("Validator: error: expected expression but found NULL\n");
        return false;
    }

    return validators[exp->crit & CRITERIA_ORDER_MASK](filename, filestat, exp);
}

/** Validates a file against an expression. */
bool validator_validate(char *filename, struct stat *filestat, parser_t *exp) {
    return !exp || validate_exp_token(filename, filestat, exp);
}
