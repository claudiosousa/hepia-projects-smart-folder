#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <libgen.h>
#include "linker.h"
#include "io.h"

/**
 * Delete all invalid or not found link in the given list,
 * by comparing the realpath of the link and the full path of the found files.
 * @param dst_path Where to put the links
 * @param files List of files to link
 */
void linker_purge(char *dst_path, finder_t *files) {
    io_file_list * links = io_directory_get_all(dst_path);
    bool found = false;
    char link_del[IO_PATH_MAX_SIZE] = "";
    char real_link_del[IO_PATH_MAX_SIZE] = "";
    char real_dst_filepath[IO_PATH_MAX_SIZE] = "";

    for (unsigned int i = 0; i < links->count; i++) {
        found = false;
        sprintf(link_del, "%s%c%s", dst_path, IO_PATH_SEP, links->files[i]);
        realpath(link_del, real_link_del);

        for (unsigned int j = 0; (j < files->count) && (!found); j++) {
            realpath(files->files[j], real_dst_filepath);

            found = (strncmp(real_link_del, real_dst_filepath, IO_PATH_MAX_SIZE) == 0);
        }

        if (!found) {
            if (io_file_delete(link_del) != 0) {
                fprintf(stderr, "Linker error: cannot purge '%s'\n", link_del);
            }
        }
    }

    io_directory_get_all_free(links);
}

void linker_update(char *dst_path, finder_t *files) {
    char filename_final[IO_PATH_MAX_SIZE] = "";
    char filepath_final[IO_PATH_MAX_SIZE] = "";
    char real_dst_filepath[IO_PATH_MAX_SIZE] = "";
    unsigned int dup_count = 0;

    for (unsigned int i = 0; i < files->count; i++) {
        strncpy(filename_final, basename(files->files[i]), IO_PATH_MAX_SIZE);

        // Count number of file that has the same name before it (in the list)
        dup_count = 0;
        for (unsigned int j = 0; j < i; j++) {
            if (strncmp(basename(files->files[j]), filename_final, IO_PATH_MAX_SIZE) == 0) {
                dup_count++;
            }
        }

        if (dup_count > 0) {
            sprintf(filename_final, "%s.%d", basename(files->files[i]), dup_count);
        }
        sprintf(filepath_final, "%s%c%s", dst_path, IO_PATH_SEP, filename_final);

        if (!io_link_exists(filepath_final)) {
            realpath(files->files[i], real_dst_filepath);
            symlink(real_dst_filepath, filepath_final);
        }

        printf("File found: %s | %s | %d\n", files->files[i], filename_final, dup_count);
    }
    printf("====== ITERATION FINISHED =======\n");

    linker_purge(dst_path, files);
}
