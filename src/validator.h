#ifndef VALIDATOR_H
#define VALIDATOR_H

#include <stdbool.h>
#include "parser.h"

struct validator_t;
typedef struct validator_t validator_t;

validator_t *validator_create(parser_t *parser_exp);
bool validator_validate(char *filename, validator_t *validator);
void validator_free(validator_t *validator);

#endif
