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
#include "io.h"

/**
 * Prints program usage
 * @param prog_name Program name
 */
void print_usage(char *prog_name) {
    fprintf(stderr, "Error: incorrect arguments\n");

    printf("Usage:\n");
    printf("\t%s <dir_name> <search_path> [expression]\n", prog_name);
    printf("\t%s -d <dir_name>\n", prog_name);
}

/**
 * Program entry-point
 * @param argc Number of arguments
 * @param argv Array of arguments
 */
int main(int argc, char *argv[]) {
    if (argc < 2) {
        print_usage(argv[0]);
        return EXIT_FAILURE;
    }

    // Search mode
    if (strncmp(argv[1], "-d", 3) != 0) {
        char *dst_path = argv[1];
        char *search_path = argv[2];
        char dst_path_abs[IO_PATH_MAX_SIZE] = "";
        char search_path_abs[IO_PATH_MAX_SIZE] = "";
        realpath(dst_path, dst_path_abs);
        realpath(search_path, search_path_abs);

        // Fork
        pid_t child_pid = fork();
        if (child_pid == -1) {
            perror("Fork failed");
            return EXIT_FAILURE;
        } else if (child_pid != 0) {
            return EXIT_SUCCESS;
        }

        // Start the search
        parser_t *expression = NULL;
        if (argc > 3) {
            expression = parser_parse(argv + 3, argc - 3);
            if (expression == NULL) {
                print_usage(argv[0]);
                return EXIT_FAILURE;
            }
        }
        smart_folder_t *smart_folder = smart_folder_create(dst_path_abs, search_path_abs, expression);
        if (smart_folder == NULL)
            return EXIT_FAILURE;

        // Setup watch
        if (ipc_set_watch(dst_path_abs, (ipc_stop_callback)smart_folder_stop, smart_folder)) {
            return EXIT_FAILURE;
        }

        smart_folder_start(smart_folder);

        if (expression != NULL)
            parser_free(expression);

    }
    // Kill mode
    else {
        char *dst_path = argv[2];
        char dst_path_abs[IO_PATH_MAX_SIZE] = "";
        realpath(dst_path, dst_path_abs);

        if (ipc_stop_watch(dst_path_abs)) {
            return EXIT_FAILURE;
        }
    }

    return EXIT_SUCCESS;
}
