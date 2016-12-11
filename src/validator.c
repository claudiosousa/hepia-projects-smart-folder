#include "validator.h"

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

bool validator_validate(char *filename, parser_t *validator)
{
    (void)filename;
    (void)validator;

    return true;
}
