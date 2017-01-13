#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include "finder.h"
#include "io.h"
#include "vendor/uthash.h"

// remove MAX_FILES
const int MAX_FILES = 10000;

static struct file_t *files = NULL;
typedef struct file_t {
    long unsigned int id; /* the key */
    UT_hash_handle hh;    /* makes this structure hashable */
} file_t;

// replace by inline handling on links
static void finder_add_found_file(char *file, finder_t *finder_files) {
    char *file_cpy = malloc(sizeof(char) * (strlen(file) + 1));
    strcpy(file_cpy, file);
    finder_files->files[finder_files->count++] = file_cpy;
}

static bool finder_hash_exist(long unsigned int inode) {
    file_t *f;
    HASH_FIND_INT(files, &inode, f);
    return f != NULL;
}

static void finder_hash_add(long unsigned int inode) {
    file_t *file = malloc(sizeof(file_t));
    file->id = inode;
    HASH_ADD_INT(files, id, file); /* id: name of key field */
}

static void finder_hash_clear() {
    file_t *current_file, *tmp;

    HASH_ITER(hh, files, current_file, tmp) {
        HASH_DEL(files, current_file);
        free(current_file);
    }

    files = NULL;
}

static void finder_find_in_dir(char *dir, finder_t *finder_files, parser_t *expression) {
    DIR *d = opendir(dir);
    if (d == NULL) {
        perror("Failled to open directory");
        return;
    }

    struct stat *file_stat = malloc(sizeof(struct stat));
    stat(dir, file_stat);

    if (finder_hash_exist(file_stat->st_ino))
        return;

    finder_hash_add(file_stat->st_ino);

    struct dirent *dent;
    char full_path[IO_PATH_MAX_SIZE];
    while ((dent = readdir(d)) != NULL) {
        if (finder_hash_exist(dent->d_ino))
            continue;

        strcpy(full_path, dir);
        strcat(full_path, "/");
        strcat(full_path, dent->d_name);
        if (dent->d_type == DT_DIR) {
            if (strcmp(dent->d_name, ".") == 0 || strcmp(dent->d_name, "..") == 0)
                continue;
            finder_find_in_dir(full_path, finder_files, expression);
        } else if (dent->d_type == DT_LNK) {
            stat(full_path, file_stat);
            if (S_ISDIR(file_stat->st_mode))
                finder_find_in_dir(full_path, finder_files, expression);
            else if (validator_validate(dent->d_name, file_stat, expression)) {
                if (finder_hash_exist(file_stat->st_ino))
                    continue;

                finder_hash_add(file_stat->st_ino);
                finder_add_found_file(full_path, finder_files);
            }

        } else if (dent->d_type == DT_REG) {
            stat(full_path, file_stat);
            if (finder_hash_exist(file_stat->st_ino))
                continue;
            if (validator_validate(dent->d_name, file_stat, expression)) {
                finder_hash_add(dent->d_ino);
                finder_add_found_file(full_path, finder_files);
            }
        }
    }

    free(file_stat);
    closedir(d);
}

finder_t *finder_find(char *search_path, parser_t *expression) {
    finder_t *finder_files = (finder_t *)malloc(sizeof(finder_t));
    finder_files->files = (char **)malloc(sizeof(char *) * MAX_FILES);
    finder_files->count = 0;

    finder_find_in_dir(search_path, finder_files, expression);

    finder_hash_clear();
    return finder_files;
}

void finder_free(finder_t *finder) {
    for (uint i = 0; i < finder->count; i++) {
        free(finder->files[i]);
    }
    free(finder->files);
    free(finder);
}
