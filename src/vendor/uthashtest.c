#include <stdio.h>
#include <dirent.h>
#include "uthash.h"

typedef struct file_t {
    long unsigned int id; /* the key */
    char name[100];
    UT_hash_handle hh; /* makes this structure hashable */
} file_t;

struct file_t *files = NULL;

void add_file(file_t *file) {
    HASH_ADD_INT(files, id, file);/* id: name of key field */
}

file_t *find_file(int file_id) {
    file_t *f;

    HASH_FIND_INT(files, &file_id, f);
    return f;
}

void delete_file(file_t *file) {
    HASH_DEL(files, file);
}

void show_file(long unsigned int file_id) {
    file_t *temp;
    temp = find_file(file_id);
    printf("Search %lu: ", file_id);
    if (temp == NULL)
        printf("NOT_FOUND\n");
    else
        printf("%s\n", temp->name);
}

void delete_all() {
  file_t *current_file, *tmp;

  HASH_ITER(hh, files, current_file, tmp) {
    HASH_DEL(files,current_file);  /* delete; users advances to next */
    free(current_file);            /* optional- if you want to free  */
  }
}

void main() {
    DIR *d = opendir(".");
    if (d == NULL) {
        perror("Failed to open directory");
        return;
    }

    struct dirent *dent;
    long unsigned int fileIds[100];
    int foundFiles = 0;
    file_t *file;
    while ((dent = readdir(d)) != NULL) {
        file = (file_t *)malloc(sizeof(file_t));
        file->id = dent->d_ino;
        strcpy(file->name, dent->d_name);
        fileIds[foundFiles++] = file->id;
        add_file(file);
        printf("(List: %lu %s)\n", dent->d_ino, dent->d_name);
    }

    closedir(d);

    for (int i = 0; i < foundFiles; i++) {
        show_file(fileIds[i]);
    }

    delete_all();
}