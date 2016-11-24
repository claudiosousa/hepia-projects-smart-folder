#include "file_validator.h"

struct file_validator_t
{

};

// name exact:    -name  <filename>
// name contains: -name -<filename>
// group exact: -group root
// user exact: -user claudio
// perm min: -perm -000
// perm exact: -perm 000
// size: -size (+-)n(GMkc)
// format numerique: n, +n,-name

//DATE (ref NOW):
// access time minutes: -atime (+-)<minutes>m
// access time days:    -atime (+-)<days>d
// modified time minutes/days: -mtime (+-)<minutes>m
// creation time minutes/days: -ctime (+-)<minutes>m

//operators:
// negation: -not
// and: -and
// or: -or
// expr: '\(...\)'


file_validator_t *file_validator_create(char *expression[], size_t size)
{
    (void)expression;
    (void)size;

    return NULL;
}

bool file_validator_validate(char *filename, file_validator_t *validator)
{
    (void)filename;
    (void)validator;

    return true;
}
