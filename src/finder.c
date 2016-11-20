#include "finder.h"

finder_files_t *finder_find(char *search_path, file_validator_t *validator) {
    (void)validator;
    (void)search_path;

    finder_files_t *finder_files = (finder_files_t *)malloc(sizeof(finder_files_t));
    char **files = (char **)malloc(sizeof(char *));
    char *str = (char *)malloc(sizeof(char) * 10);
    strncpy(str, "test", 5);
    *files = str;
    finder_files->files = files;
    finder_files->count = 1;

    return finder_files;
}

void finder_free(finder_files_t *finder) { (void)finder; }
