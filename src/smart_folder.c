#include <stdio.h>
#include <sys/stat.h>
#include <unistd.h>

#include "smart_folder.h"
#include "file_linker.h"
#include "finder.h"

const int LOOP_INTERVAL = 5;  // seconds

struct smart_folder_t {
    bool running;
    char* dst_path;
    char* search_path;
    file_validator_t* validator;
};

smart_folder_t* smart_folder_create(char* dst_path, char* search_path, file_validator_t* validator) {
    struct stat dirstat;

    if (stat(dst_path, &dirstat) == 0) {
        fprintf(stderr, "Destination already exists!");
        return NULL;
    }
    if (stat(search_path, &dirstat) != 0 || !S_ISDIR(dirstat.st_mode)) {
        fprintf(stderr, "Search path does not exist or is not a directory!");
        return NULL;
    }
    if (mkdir(dst_path, S_IRWXU | S_IRWXG) != 0) {
        perror("Impossible to create destination path!");
        return NULL;
    }

    smart_folder_t* smart_folder = (smart_folder_t*)malloc(sizeof(smart_folder_t));
    smart_folder->running = false;
    smart_folder->dst_path = dst_path;
    smart_folder->search_path = search_path;
    smart_folder->validator = validator;

    return smart_folder;
}

void smart_folder_start(smart_folder_t* smart_folder) {
    smart_folder->running = true;
    while (smart_folder->running) {
        finder_files_t* found_files = finder_find(smart_folder->search_path, smart_folder->validator);
        file_linker_update(smart_folder->dst_path, found_files);
        sleep(LOOP_INTERVAL);
    }
}

void smart_folder_stop(smart_folder_t* smart_folder) {
    smart_folder->running = false;
    if (rmdir(smart_folder->dst_path) != 0) {
        perror("Impossible to delete smart_folder path!");
    }
}
