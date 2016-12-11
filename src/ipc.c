/**
 * Manage communication between different instance of smartfolder.
 * @author Claudio Sousa, Gonzalez David
 */

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <inttypes.h>
#include "ipc.h"
#include "io.h"

#define IPC_MAX_PID_SIZE 10
#define IPC_HOME_PATH "/.smartfolder/"
#define IPC_RUN_PATH "/run/"
#define IPC_HOME_RUN_PATH IPC_HOME_PATH IPC_RUN_PATH

/**
 * Return the pid file path for the watch constructed from the folder.
 * The pid filename is constructed by simply replacing the '/' character by '_'
 * @param dst_path The destination path to construct the filename from
 * @param pid_path String to store the resulting pid filename
 */
 //TODO rename function, add ensure param?
static int ipc_get_pid_file_path(char *dst_path, char *pid_path) {
    // Get user home directory root
    char *home_dir = getenv("HOME");
    if (home_dir == NULL) {
        fprintf(stderr, "IPC: Error: Invalid user home directory");
        return 1;
    }

    // Create our run directory.
    // No check are done because if path already exists, nothing happen.
    strncpy(pid_path, home_dir, IO_PATH_MAX_SIZE);
    strncat(pid_path, IPC_HOME_PATH, IO_PATH_MAX_SIZE);
    mkdir(pid_path, IO_DEFAULT_MODE);

    strncat(pid_path, IPC_RUN_PATH, IO_PATH_MAX_SIZE);
    mkdir(pid_path, IO_DEFAULT_MODE);

    // Store the PID to a file named after the destination path
    int folder_length = strlen(pid_path);
    strncat(pid_path, dst_path, IO_PATH_MAX_SIZE);
    for (int i = folder_length; pid_path[i]; ++i) {
        if (pid_path[i] == '/') {
            pid_path[i] = '_';
        }
    }

    return 0;
}

int ipc_set_watch(char *dst_path) {
    char pid_path[IO_PATH_MAX_SIZE];
    if (ipc_get_pid_file_path(dst_path, pid_path) == 1) {
        return 1;
    }

    if (io_file_exists(pid_path)) {
        fprintf(stderr, "IPC: Error: there already is a watch for '%s'", dst_path);
        return 1;
    }

    // Get our PID and write it to the constructed file
    char pid_str[IPC_MAX_PID_SIZE] = "";
    sprintf(pid_str, "%d", getpid());

    if (io_file_write(pid_path, pid_str) != 0) {
        fprintf(stderr, "IPC: Error: cannot write pid to file '%s'\n", pid_path);
        return 1;
    }

    return 0;
}

int ipc_remove_watch(char *dst_path) {
    char pid_path[IO_PATH_MAX_SIZE];
    if (ipc_get_pid_file_path(dst_path, pid_path) == 1) {
        return 1;
    }

    if (io_file_delete(pid_path)) {
        fprintf(stderr, "IPC: Error: cannot delete pid file '%s'\n", pid_path);
        return 1;
    }

    return 0;
}

int ipc_stop_watch(char *dst_path) {
    char pid_path[IO_PATH_MAX_SIZE];
    if (ipc_get_pid_file_path(dst_path, pid_path) == 1) {
        return 1;
    }

    char pid_str[IPC_MAX_PID_SIZE];
    if (io_file_read_content(pid_path, pid_str, IPC_MAX_PID_SIZE) != 0) {
        fprintf(stderr, "IPC: Error: cannot get pid of instance for this path '%s'\n", dst_path);
        return 1;
    }

    unsigned int pid = strtoimax(pid_str, NULL, 10);
    if (pid == 0) {
        fprintf(stderr, "IPC: Error: invalid PID data in '%s'\n", pid_path);
        return 1;
    }

    // Kill process SIGTERM
    if (kill(pid, SIGTERM)) {
        fprintf(stderr, "IPC: Error: cannot kill process '%d'\n", pid);
        perror("kill failed");

        // smartfolder not running, we remove the watch file
        ipc_remove_watch(dst_path);
        return 1;
    }

    return 0;
}
