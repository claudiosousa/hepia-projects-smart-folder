#include <dirent.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include "finder.h"
#include "io.h"
#include "logger.h"
#include "vendor/uthash.h"

static struct file_t *files = NULL;
typedef struct file_t {
    long unsigned int id; /* the key */
    UT_hash_handle hh;    /* makes this structure hashable */
} file_t;

static finder_t *finder_add_found_file(char *filename, finder_t *currentfile) {
    finder_t *newfile = (finder_t *)malloc(sizeof(finder_t));
    char *filename_cpy = malloc(sizeof(char) * (strlen(filename) + 1));
    strcpy(filename_cpy, filename);
    newfile->filename = filename_cpy;
    newfile->next = currentfile;
    return newfile;
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

static finder_t *finder_find_in_dir(char *dir, finder_t *filelist, parser_t *expression) {
    DIR *d = opendir(dir);
    if (d == NULL) {
        logger_perror("Finder: error: failed to open directory");
        return filelist;
    }

    struct stat *file_stat = malloc(sizeof(struct stat));
    stat(dir, file_stat);

    if (finder_hash_exist(file_stat->st_ino))
        return filelist;

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
            filelist = finder_find_in_dir(full_path, filelist, expression);
        } else if (dent->d_type == DT_LNK) {
            stat(full_path, file_stat);
            if (S_ISDIR(file_stat->st_mode))
                filelist = finder_find_in_dir(full_path, filelist, expression);
            else if (validator_validate(dent->d_name, file_stat, expression)) {
                if (finder_hash_exist(file_stat->st_ino))
                    continue;

                finder_hash_add(file_stat->st_ino);
                filelist = finder_add_found_file(full_path, filelist);
            }

        } else if (dent->d_type == DT_REG) {
            stat(full_path, file_stat);
            if (finder_hash_exist(file_stat->st_ino))
                continue;
            if (validator_validate(dent->d_name, file_stat, expression)) {
                finder_hash_add(dent->d_ino);
                filelist = finder_add_found_file(full_path, filelist);
            }
        }
    }

    free(file_stat);
    closedir(d);

    return filelist;
}

finder_t *finder_find(char *search_path, parser_t *expression) {
    finder_t *foundfiles = finder_find_in_dir(search_path, NULL, expression);

    finder_hash_clear();
    return foundfiles;
}

void finder_free(finder_t *finder) {
    finder_t *previous;
    while (finder) {
        previous = finder;
        finder = finder->next;
        free(previous->filename);
        free(previous);
    }
}
