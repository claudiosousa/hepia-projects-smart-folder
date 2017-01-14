/**
   The finder retrives the list of files matching an expression

   A specified is search resursively for all find valid against a give filtering expression.

   Symbolic links are followed, and an hashtable is kept with the id's (inode id) of the processed paths to avoid
   processing the same paths twice and avoid infinte loops.

   @file
 */

#include <dirent.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include "finder.h"
#include "io.h"
#include "logger.h"
#include "vendor/uthash.h"

/** Hashtable containing the processed files.
    Used to avoid processing same paths twice.
    @see file_t
 */
static struct file_t *files = NULL;

/** Hashtable entry type used in `files`.
    Each instance contains the inode id of a path already processed
    @see files
 */
typedef struct file_t {
    long unsigned int id; /**< Inode id */
    UT_hash_handle hh;    /**< Makes this structure hashable */
} file_t;

/** Adds the found valid file to the list of found files
    @param filename Found file's name
    @param currentfile Chained list of found files
    @returns New pointer to the found files chained list
 */
static finder_t *finder_add_found_file(char *filename, finder_t *currentfile) {
    finder_t *newfile = (finder_t *)malloc(sizeof(finder_t));
    char *filename_cpy = malloc(sizeof(char) * IO_PATH_MAX_SIZE);
    realpath(filename, filename_cpy);
    newfile->filename = filename_cpy;
    newfile->next = currentfile;
    return newfile;
}

/** Verifies if an inode id has already been processed */
static bool finder_hash_exist(long unsigned int inode) {
    file_t *f;
    HASH_FIND_INT(files, &inode, f);
    return f != NULL;
}

/** Adds an inode id to the hashtable of processed files */
static void finder_hash_add(long unsigned int inode) {
    file_t *file = malloc(sizeof(file_t));
    file->id = inode;
    HASH_ADD_INT(files, id, file); /* id: name of key field */
}

/** Clears (frees) the processed files hashtable  */
static void finder_hash_clear() {
    file_t *current_file, *tmp;

    HASH_ITER(hh, files, current_file, tmp) {
        HASH_DEL(files, current_file);
        free(current_file);
    }

    files = NULL;
}

static finder_t *finder_find_in_dir(char *dir, finder_t *filelist, parser_t *expression);

/** Processes a found directory entity.

   If it is a *directory*:
    - we use `finder_find_in_dir` to analyze the content

   If it is a *regular file*:
     - we check that if has not been processed yet
     - we check if is valid file against the expression
     - if the two previous conditions are met:
       + it is added to the hashtable of processed files
       + it is added to the list of valid files found

   If it is a *symbolic link*, we retrieve the target and check the target type:
    - if it is a *directory, we proceed with the above conditions for directories
    - otherwise we proceed with above conditions for regular files

   @param dent Directory entity to process
   @param dirpath Directory entity full path
   @param filelist Pointer to the found files chained list
   @param expression Filtering expression
   @returns New pointer to the found files chained list
 */
static finder_t *finder_process_dent(struct dirent *dent, char *dirpath, finder_t *filelist, parser_t *expression) {
    struct stat file_stat;

    switch (dent->d_type) {
        case DT_DIR:
            if (strcmp(dent->d_name, ".") != 0 && strcmp(dent->d_name, "..") != 0)
                filelist = finder_find_in_dir(dirpath, filelist, expression);
            break;
        case DT_LNK:
            stat(dirpath, &file_stat);
            if (S_ISDIR(file_stat.st_mode))
                filelist = finder_find_in_dir(dirpath, filelist, expression);
            else if (validator_validate(dent->d_name, &file_stat, expression)) {
                if (!finder_hash_exist(file_stat.st_ino)) {
                    finder_hash_add(file_stat.st_ino);
                    filelist = finder_add_found_file(dirpath, filelist);
                }
            }
            break;
        case DT_REG:
            stat(dirpath, &file_stat);
            if (!finder_hash_exist(file_stat.st_ino) && validator_validate(dent->d_name, &file_stat, expression)) {
                finder_hash_add(dent->d_ino);
                filelist = finder_add_found_file(dirpath, filelist);
            }
    }

    return filelist;
}

/** Searches a directory recursively for files matching the expression

    Iterates over the directory entities and delegates their processing to `finder_process_dent`.
    Directories are added to the hastable of processed paths.
    @param dir Directory full path
    @param filelist Pointer to the found files chained list
    @param expression Filtering expression
    @returns New pointer to the found files chained list
*/
static finder_t *finder_find_in_dir(char *dir, finder_t *filelist, parser_t *expression) {
    DIR *d = opendir(dir);
    if (d == NULL) {
        logger_perror("Finder: error: failed to open directory");
        return filelist;
    }

    struct stat file_stat;
    stat(dir, &file_stat);

    if (finder_hash_exist(file_stat.st_ino))
        return filelist;

    finder_hash_add(file_stat.st_ino);

    struct dirent *dent;
    char full_path[IO_PATH_MAX_SIZE];
    while ((dent = readdir(d)) != NULL) {
        if (finder_hash_exist(dent->d_ino))
            continue;

        strcpy(full_path, dir);
        strcat(full_path, "/");
        strcat(full_path, dent->d_name);

        filelist = finder_process_dent(dent, full_path, filelist, expression);
    }

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
