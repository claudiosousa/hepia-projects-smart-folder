#ifndef VALIDATOR_H
#define VALIDATOR_H

#include <stdbool.h>
#include <sys/stat.h>
#include "parser.h"

bool validator_validate(char *filename, struct stat *filestat, parser_t *expression);

#endif
