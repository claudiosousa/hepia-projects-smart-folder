#include <stdlib.h>
#include "file_validator.h"

typedef struct {
    char *files;
    size_t count;
} finder_t;

finder_t *finder_find(char *search_path, file_validator_t *validator);
void finder_free(finder_t *finder);
