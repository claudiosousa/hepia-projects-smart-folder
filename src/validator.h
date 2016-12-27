#ifndef VALIDATOR_H
#define VALIDATOR_H

#include <stdbool.h>
#include "parser.h"

bool validator_validate(char *filename, parser_t *expression);

#endif
