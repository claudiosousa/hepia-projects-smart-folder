/** Validates files against a parsed criteria expression.
    @file
 */
#ifndef VALIDATOR_H
#define VALIDATOR_H

#include <stdbool.h>
#include <sys/stat.h>
#include "parser.h"

/** Validates a file against an expression

    @param filename The file's name
    @param filestat The file's attributes
    @param expression The expression used to validate the file
    @returns If the file is valid
 */
bool validator_validate(char *filename, struct stat *filestat, parser_t *expression);

#endif
