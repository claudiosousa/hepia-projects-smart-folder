/**
 * The linker maintains an updated list of links to the found files.
 *
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
 * @file
 */

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <inttypes.h>
#include <libgen.h>
#include "linker.h"
#include "io.h"
#include "logger.h"

/**
 * Count the number of times the current file appear in the list before itself.
 * @param current_file Current treated file in the below list
 * @param files List of files to link
 * @return Number of times it appears
 */
unsigned int linker_get_count(finder_t *current_file, finder_t *files) {
    unsigned int dup_count = 0;

    while (files != current_file) {
        if (strncmp(basename(files->filename), basename(current_file->filename), IO_PATH_MAX_SIZE) == 0) {
            dup_count++;
        }
        files = files->next;
    }

    return dup_count;
}

/**
 * Retreive the duplicate number from the filename
 * @param link_filename Filename of the link
 * @return Appending duplicate counter
 */
unsigned int linker_get_count_from_filename(char *link_filename) {
    unsigned int dup_count = 0;

    const char *dot = strrchr(link_filename, '.');
    if ((dot != NULL) && (dot != link_filename)) {
        const char *ext = dot + 1;
        dup_count = strtoimax(ext, NULL, 10);
    }

    return dup_count;
}

/**
 * Delete all links in the destination folder that are no longer in the given list.
 * To determine whether a file is no longer here, it searchs the target of the link in the list.
 * If it is not found, the linker delete the link.
 * @param dst_path Where to put the links
 * @param files List of files to link
 */
void linker_purge(char *dst_path, finder_t *files) {
    io_file_list_t *links = io_directory_get_all(dst_path);
    bool found = false;
    char link_del[IO_PATH_MAX_SIZE] = "";
    char link_del_target[IO_PATH_MAX_SIZE] = "";
    char real_dst_filepath[IO_PATH_MAX_SIZE] = "";
    finder_t *file;

    io_file_list_t *workinglink = links;
    while (workinglink) {
        found = false;
        snprintf(link_del, IO_PATH_MAX_SIZE, "%s%c%s", dst_path, IO_PATH_SEP, workinglink->file);
        realpath(link_del, link_del_target);

        file = files;
        while (file && !found) {
            realpath(file->filename, real_dst_filepath);
            found = (strncmp(link_del_target, real_dst_filepath, IO_PATH_MAX_SIZE) == 0);

            // If found, don't do next because we need this reference for duplicate count
            if (!found) {
                file = file->next;
            }
        }
        // Avoid crash if the current file is not found
        if (!found) {
            file = files;
        }

        if ((!found) || (linker_get_count_from_filename(link_del) != linker_get_count(file, files))) {
            logger_debug("Purge: %s | %u | %u\n", link_del, linker_get_count_from_filename(link_del),
                         linker_get_count(file, files));
            if (io_file_delete(link_del) != 0) {
                logger_error("Linker error: cannot purge '%s'\n", link_del);
            }
        } else {
            logger_debug("Don't purge: %s | %u | %u\n", link_del, linker_get_count_from_filename(link_del),
                         linker_get_count(file, files));
        }
        workinglink = workinglink->next;
    }

    io_directory_get_all_free(links);
}

void linker_update(char *dst_path, finder_t *files) {
    char filename_final[IO_PATH_MAX_SIZE] = "";
    char filepath_final[IO_PATH_MAX_SIZE] = "";
    char real_dst_filepath[IO_PATH_MAX_SIZE] = "";
    unsigned int dup_count = 0;
    finder_t *file = files;

    linker_purge(dst_path, files);

    while (file) {
        logger_debug("File found '%s'\n", file->filename);

        dup_count = linker_get_count(file, files);
        strncpy(filename_final, basename(file->filename), IO_PATH_MAX_SIZE);

        if (dup_count > 0) {
            snprintf(filename_final, IO_PATH_MAX_SIZE, "%s.%d", basename(file->filename), dup_count);
        }
        snprintf(filepath_final, IO_PATH_MAX_SIZE, "%s%c%s", dst_path, IO_PATH_SEP, filename_final);

        if (!io_link_exists(filepath_final)) {
            logger_debug("Create link '%s' | %s | %d\n", file->filename, filename_final, dup_count);
            realpath(file->filename, real_dst_filepath);
            symlink(real_dst_filepath, filepath_final);
        }

        file = file->next;
    }

    logger_debug("====== ITERATION FINISHED =======\n");
}
