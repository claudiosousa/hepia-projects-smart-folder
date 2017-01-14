/**
 * Manage communication between different instance of smartfolder.
 *
 * In order to do this, during initialisation (ipc_set_watch),
 * it create a file in the user home directory and put the PID of the current
 * instance in it. The name of this file is constructed by taking the
 * provided destination path and replacing all directory separator by an underscore.
 *
 * It also setup two signals catch (SIGINT and SIGTERM) for cleanup purpose.
 * The callback and callback data are provided by the caller, and stored
 * as global for usage in the signl handler. This is this handler that also remove the
 * file in the user home directory.
 *
 * Now, to terminate an instance, it gets the PID of the target instance
 * by getting it in the concerned file. To get this file, it does the
 * same string replacement in the provided destination folder to have the right name.
 * Then, with the PID, it launch a SIGTERM signal to the instance.
 *
 * When a signal is received, the signal handler will first remove the file
 * and call the callback if any.
 *
 * @author Claudio Sousa, Gonzalez David
 */

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <inttypes.h>
#include "ipc.h"
#include "io.h"
#include "logger.h"

#define IPC_MAX_PID_SIZE 10
#define IPC_HOME_PATH "/.searchfolder/"
#define IPC_RUN_PATH "/run/"
#define IPC_HOME_RUN_PATH IPC_HOME_PATH IPC_RUN_PATH

/**
 * Destination path on which this instance operates on.
 */
static char g_watch_dst_path[IO_PATH_MAX_SIZE] = "";
/**
 * Provided callback when a signal is received
 */
static ipc_stop_callback g_watch_cb = NULL;
/**
 * Callback data for the callback when signal is received
 */
static void * g_watch_cb_arg = NULL;

/**
 * Return the pid file path for the watch constructed from the folder.
 * The pid filename is constructed by simply replacing the '/' character by '_'
 * @param dst_path The destination path to construct the filename from
 * @param pid_path String to store the resulting pid filename
 */
static int ipc_get_pid_file_path(char *dst_path, char *pid_path) {
    // Get user home directory root
    char *home_dir = getenv("HOME");
    if (home_dir == NULL) {
        logger_perror("IPC: Error: Invalid user home directory");
        return 1;
    }
    strncpy(pid_path, home_dir, IO_PATH_MAX_SIZE);
    strncat(pid_path, IPC_HOME_RUN_PATH, IO_PATH_MAX_SIZE);

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

/**
 * IPC signal handler for SIGTERM|SIGINT that remove the watch
 * and optionally call a callback.
 */
void ipc_sig_handler() {
    ipc_remove_watch(g_watch_dst_path);
    if (g_watch_cb != NULL) {
        g_watch_cb(g_watch_cb_arg);
    }
}

int ipc_set_watch(char *dst_path, ipc_stop_callback cb, void * cb_arg) {
    // Setup signal
    struct sigaction act;
    act.sa_handler = ipc_sig_handler;
    act.sa_flags = 0;
    sigemptyset(&act.sa_mask);
    if ((sigaction(SIGINT, &act, NULL) == -1) || (sigaction(SIGTERM, &act, NULL) == -1)) {
        logger_perror("IPC: ERROR: failed to register signal");
        return 1;
    }
    strncpy(g_watch_dst_path, dst_path, IO_PATH_MAX_SIZE);
    g_watch_cb = cb;
    g_watch_cb_arg = cb_arg;

    // Get file where to write our PID
    char pid_path[IO_PATH_MAX_SIZE];
    if (ipc_get_pid_file_path(dst_path, pid_path) == 1) {
        return 1;
    }

    // Create our run directory
    io_directory_create_parent(pid_path);

    if (io_file_exists(pid_path)) {
        logger_error("IPC: Error: there already is a watch for '%s'\n", dst_path);
        return 1;
    }

    // Get our PID and write it to the constructed file
    char pid_str[IPC_MAX_PID_SIZE] = "";
    snprintf(pid_str, IPC_MAX_PID_SIZE, "%d", getpid());

    if (io_file_write(pid_path, pid_str) != 0) {
        logger_error("IPC: Error: cannot write pid to file '%s'\n", pid_path);
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
        logger_error("IPC: Error: cannot delete pid file '%s'\n", pid_path);
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
        logger_error("IPC: Error: cannot get pid of instance for this path '%s'\n", dst_path);
        return 1;
    }

    unsigned int pid = strtoimax(pid_str, NULL, 10);
    if (pid == 0) {
        logger_error("IPC: Error: invalid PID data in '%s'\n", pid_path);
        return 1;
    }

    // Kill process SIGTERM
    if (kill(pid, SIGTERM)) {
        logger_error("IPC: Error: cannot kill process '%d'\n", pid);
        logger_perror("  ");

        // smartfolder not running, we remove the watch file
        ipc_remove_watch(dst_path);
        return 1;
    }

    return 0;
}
