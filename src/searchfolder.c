#include <stdio.h>
#include <sys/stat.h>
#include <unistd.h>
#include "searchfolder.h"
#include "linker.h"
#include "finder.h"
#include "io.h"

const int LOOP_INTERVAL = 5;  // seconds

struct searchfolder_t {
    bool running;
    char* dst_path;
    char* search_path;
    parser_t* expression;
};

searchfolder_t* searchfolder_create(char* dst_path, char* search_path, parser_t* expression) {
    if (io_directory_exists(dst_path)) {
        fprintf(stderr, "Destination already exists!");
        return NULL;
    }
    if (!io_directory_exists(search_path)) {
        fprintf(stderr, "Search path does not exist or is not a directory!");
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
        fprintf(stderr, "Impossible to create destination path!");
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
        fprintf(stderr, "Impossible to delete searchfolder path");
    }
    free(searchfolder);
}

void searchfolder_stop(searchfolder_t* searchfolder) {
    searchfolder->running = false;
}