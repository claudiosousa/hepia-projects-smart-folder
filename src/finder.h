#ifndef FINDER_H
#define FINDER_H

#include <stdlib.h>
#include "validator.h"

typedef struct {
    char **files;
    size_t count;
} finder_t;

finder_t *finder_find(char *search_path, parser_t *expression);
void finder_free(finder_t *finder);

#endif
