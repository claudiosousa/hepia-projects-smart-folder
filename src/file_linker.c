#include <stdio.h>
#include "file_linker.h"

void file_linker_update(char *dst_path, finder_files_t *files) {
    (void)dst_path;
    (void)files;

    for (int i; i < files->count; i++) {
        fprintf(stdout, "File found: %s\n", files->files[i]);
    }
}
