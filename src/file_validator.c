#include "file_validator.h"

struct file_validator_t
{

};

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
