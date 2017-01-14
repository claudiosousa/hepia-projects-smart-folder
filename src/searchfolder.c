/** Orchestrates the file search and creation of related symbolic links

    This is the main module.

    Once started it runs continously. It uses the `finder` module to get the files matching the `expression` within `search_path`.
    The resulting list is past to the `linker` module to update the `dst_path`
    It will wait a few seconds, and it will start over.

    @file
 */
#include <sys/stat.h>
#include <unistd.h>
#include "searchfolder.h"
#include "linker.h"
#include "finder.h"
#include "io.h"
#include "logger.h"

/** The time in seconds to wait between to executions */
#define LOOP_INTERVAL 5  // seconds

/** Contains the information about a searchfolder instance */
struct searchfolder_t {
    bool running;          /**< If it is running */
    char* dst_path;        /**< The output folder */
    char* search_path;     /**< The search folder */
    parser_t* expression;  /**< The file filtering expression */
};

searchfolder_t* searchfolder_create(char* dst_path, char* search_path, parser_t* expression) {
    if (io_directory_exists(dst_path)) {
        logger_error("Destination '%s' already exists\n", dst_path);
        return NULL;
    }
    if (!io_directory_exists(search_path)) {
        logger_error("Search path '%s' does not exist or is not a directory\n", search_path);
        return NULL;
    }

    searchfolder_t* searchfolder = (searchfolder_t*)malloc(sizeof(searchfolder_t));
    searchfolder->running = false;
    searchfolder->dst_path = dst_path;
    searchfolder->search_path = search_path;
    searchfolder->expression = expression;

    return searchfolder;
}

void searchfolder_start(searchfolder_t* searchfolder) {
    if (io_directory_create(searchfolder->dst_path) != 0) {
        logger_error("Impossible to create destination path '%s'\n", searchfolder->dst_path);
        return;
    }

    searchfolder->running = true;

    while (searchfolder->running) {
        finder_t* found_files = finder_find(searchfolder->search_path, searchfolder->expression);
        linker_update(searchfolder->dst_path, found_files);

        finder_free(found_files);
        sleep(LOOP_INTERVAL);
    }

    if (io_directory_delete(searchfolder->dst_path) != 0) {
        logger_error("Impossible to delete destination path '%s'\n", searchfolder->dst_path);
    }
    free(searchfolder);
}

void searchfolder_stop(searchfolder_t* searchfolder) {
    searchfolder->running = false;
}
