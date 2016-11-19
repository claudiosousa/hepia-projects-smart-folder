/**
 * Main program: deamonise, parse argument and launch the search.
 * @author Claudio Sousa, Gonzalez David
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ipc.h"
#include "file_validator.h"
#include "smart_folder.h"

/**
 * Program entry-point
 * @param argc Number of arguments
 * @param argv Array of arguments
 */
int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        fprintf(stderr, "Error: missing arguments\n");

        printf("Usage:\n");
        printf("       %s <dir_name> <search_path> [expression]", argv[0]);
        printf("       %s -d <dir_name>", argv[0]);

        return EXIT_FAILURE;
    }

    // Search mode
    if (strncmp(argv[1], "-d", 3) != 0)
    {
        char * dst_path = argv[1];
        char * search_path = argv[2];

        if (ipc_set_watch(dst_path))
        {
            return EXIT_FAILURE;
        }

        // Start the search
        file_validator_t * validator = file_validator_create((argc > 2) ? argv+3 : NULL, argc - 3);
        smart_folder_start(dst_path, search_path, validator);
    }
    // Kill mode
    else
    {
        char * dst_path = argv[2];

        if (ipc_stop_watch(dst_path))
        {
            return EXIT_FAILURE;
        }

        smart_folder_stop(dst_path);
    }

    return EXIT_SUCCESS;
}
