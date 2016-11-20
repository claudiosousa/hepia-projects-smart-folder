#include <stdio.h>
#include <dirent.h>
#include <string.h>

#include "finder.h"

const int MAX_FILES = 10000;
const int MAX_PATH = 4096;

static void add_found_file(char *file, finder_files_t *finder_files) {
    char *file_cpy = malloc(sizeof(char) * (strlen(file) + 1));
    strcpy(file_cpy, file);
    finder_files->files[finder_files->count++] = file_cpy;
}

static void find_in_dir(char *dir, finder_files_t *finder_files, file_validator_t *validator) {
    (void)validator;
    (void)finder_files;

    DIR *d;
    d = opendir(dir);
    if (d == NULL) {
        perror("Failled to open directory");
        return;
    }

    struct dirent *dent;
    char full_path[MAX_PATH];
    while ((dent = readdir(d)) != NULL) {
        strcpy(full_path, dir);
        strcat(full_path, "/");
        strcat(full_path, dent->d_name);
        if (dent->d_type == DT_DIR) {
            if (strcmp(dent->d_name, ".") == 0 || strcmp(dent->d_name, "..") == 0) continue;
            find_in_dir(full_path, finder_files, validator);
        } else if (dent->d_type == DT_REG) {
            if (file_validator_validate(full_path, validator)) {
                add_found_file(full_path, finder_files);
            }
        }
    }

    closedir(d);
}

finder_files_t *finder_find(char *search_path, file_validator_t *validator) {
    finder_files_t *finder_files = (finder_files_t *)malloc(sizeof(finder_files_t));
    finder_files->files = (char **)malloc(sizeof(char *) * MAX_FILES);
    finder_files->count = 0;

    find_in_dir(search_path, finder_files, validator);

    return finder_files;
}

void finder_free(finder_files_t *finder) {
    for (uint i = 0; i < finder->count; i++) {
        free(finder->files[i]);
    }
    free(finder->files);
    free(finder);
}
