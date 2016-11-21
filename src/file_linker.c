#include <stdio.h>
#include "file_linker.h"

//TODO return state to pass to next call to file_linker_update
void file_linker_update(char *dst_path, finder_files_t *files) {
    (void)dst_path;

    for (uint i; i < files->count; i++) {
        fprintf(stdout, "File found: %s\n", files->files[i]);
    }
}
