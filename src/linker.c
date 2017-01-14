/**
 * The linker has two jobs:
 *   - creates the links for the provided list of files found;
 *   - deletes old links that are no longer present in the list.
 *
 * In order to create the links, it simply take the base name of the target file
 * and create the link with this name in the destination folder.
 * But to manage duplicate, it count the number of files that has the same basename
 * in the list before the current one and if it is not 0, append the number to the name.
 *
 * To purge the destination folder, it gets all the links in it. Then, for each link,
 * it determines whether a file is no longer here by searching the target of the link in the list.
 * If it is not found, the linker deletes the link in the destination folder.
 *
 * @author Claudio Sousa, Gonzalez David
 */

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <libgen.h>
#include "linker.h"
#include "io.h"
#include "logger.h"

/**
 * Delete all links in the destination folder that are no longer in the given list.
 * To determine whether a file is no longer here, it searchs the target of the link in the list.
 * If it is not found, the linker delete the link.
 * @param dst_path Where to put the links
 * @param files List of files to link
 */
void linker_purge(char *dst_path, finder_t *files) {
    io_file_list *links = io_directory_get_all(dst_path);
    bool found = false;
    char link_del[IO_PATH_MAX_SIZE] = "";
    char real_link_del[IO_PATH_MAX_SIZE] = "";
    char real_dst_filepath[IO_PATH_MAX_SIZE] = "";
    finder_t *file;

    for (unsigned int i = 0; i < links->count; i++) {
        found = false;
        snprintf(link_del, IO_PATH_MAX_SIZE, "%s%c%s", dst_path, IO_PATH_SEP, links->files[i]);
        realpath(link_del, real_link_del);

        file = files;
        while (file && !found) {
            realpath(file->filename, real_dst_filepath);
            found = (strncmp(real_link_del, real_dst_filepath, IO_PATH_MAX_SIZE) == 0);
            file = file->next;
        }

        if (!found) {
            if (io_file_delete(link_del) != 0) {
                logger_error("Linker error: cannot purge '%s'\n", link_del);
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
    finder_t *file = files;
    while (file) {
        strncpy(filename_final, basename(file->filename), IO_PATH_MAX_SIZE);

        // Count number of file that has the same name before it (in the list)
        dup_count = 0;
        finder_t *filecpy = files;
        while (filecpy != file) {
            if (strncmp(basename(filecpy->filename), filename_final, IO_PATH_MAX_SIZE) == 0) {
                dup_count++;
            }
            filecpy = filecpy->next;
        }

        if (dup_count > 0) {
            snprintf(filename_final, IO_PATH_MAX_SIZE, "%s.%d", basename(file->filename), dup_count);
        }
        snprintf(filepath_final, IO_PATH_MAX_SIZE, "%s%c%s", dst_path, IO_PATH_SEP, filename_final);

        if (!io_link_exists(filepath_final)) {
            realpath(file->filename, real_dst_filepath);
            symlink(real_dst_filepath, filepath_final);
        }

        logger_debug("File found '%s' | %d\n", file->filename, dup_count);

        file = file->next;
    }
    logger_debug("====== ITERATION FINISHED =======\n");

    linker_purge(dst_path, files);
}
