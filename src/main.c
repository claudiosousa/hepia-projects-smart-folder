/**
 * Main program: deamonise, parse argument and launch the search.
 * @author Claudio Sousa, Gonzalez David
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "ipc.h"
#include "parser.h"
#include "smart_folder.h"

/**
 * Program entry-point
 * @param argc Number of arguments
 * @param argv Array of arguments
 */
int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Error: missing arguments\n");

        printf("Usage:\n");
        printf("\t%s <dir_name> <search_path> [expression]\n", argv[0]);
        printf("\t%s -d <dir_name>\n", argv[0]);

        return EXIT_FAILURE;
    }

    // Search mode
    if (strncmp(argv[1], "-d", 3) != 0) {
        char *dst_path = argv[1];
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
        smart_folder_t *smart_folder = smart_folder_create(dst_path, search_path, expression);
        if (smart_folder == NULL) return EXIT_FAILURE;

        // Setup watch
        if (ipc_set_watch(dst_path, (ipc_stop_callback)smart_folder_stop, smart_folder)) {
            return EXIT_FAILURE;
        }

        smart_folder_start(smart_folder);
    }
    // Kill mode
    else {
        char *dst_path = argv[2];

        if (ipc_stop_watch(dst_path)) {
            return EXIT_FAILURE;
        }
    }

    return EXIT_SUCCESS;
}
