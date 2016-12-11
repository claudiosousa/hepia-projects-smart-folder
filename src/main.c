/**
 * Main program: deamonise, parse argument and launch the search.
 * @author Claudio Sousa, Gonzalez David
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include "ipc.h"
#include "parser.h"
#include "smart_folder.h"

// TODO: move this somewhere else?
smart_folder_t *smart_folder;
char *dst_path;
void sig_handler() {
    if (smart_folder) {
        smart_folder_stop(smart_folder);
        ipc_remove_watch(dst_path);
    }
}

/**
 * Program entry-point
 * @param argc Number of arguments
 * @param argv Array of arguments
 */
int main(int argc, char *argv[]) {
    if (signal(SIGINT, sig_handler) == SIG_ERR || signal(SIGTERM, sig_handler) == SIG_ERR) {
        perror("Failed to register signal");
        return EXIT_FAILURE;
    }

    if (argc < 2) {
        fprintf(stderr, "Error: missing arguments\n");

        printf("Usage:\n");
        printf("\t%s <dir_name> <search_path> [expression]\n", argv[0]);
        printf("\t%s -d <dir_name>\n", argv[0]);

        return EXIT_FAILURE;
    }

    // Search mode
    if (strncmp(argv[1], "-d", 3) != 0) {
        dst_path = argv[1];
        char *search_path = argv[2];

        // Fork
        pid_t child_pid = fork();
        if (child_pid == -1) {
            perror("Fork failed");
            return EXIT_FAILURE;
        } else if (child_pid != 0) {
            return EXIT_SUCCESS;
        }

        // Start the search
        parser_t *expression = parser_parse((argc > 2) ? argv + 3 : NULL, argc - 3);
        smart_folder = smart_folder_create(dst_path, search_path, expression);
        if (smart_folder == NULL) return EXIT_FAILURE;

        if (ipc_set_watch(dst_path)) {
            return EXIT_FAILURE;
        }

        smart_folder_start(smart_folder);
    }
    // Kill mode
    else {
        dst_path = argv[2];

        if (ipc_stop_watch(dst_path)) {
            return EXIT_FAILURE;
        }
    }

    return EXIT_SUCCESS;
}
