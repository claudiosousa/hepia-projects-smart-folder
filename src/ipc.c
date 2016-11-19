/**
 * Manage communication between different instance of smartfolder.
 * @author Claudio Sousa, Gonzalez David
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include <limits.h>
#include <signal.h>
#include "ipc.h"

#define IPC_MAX_PATH 4096
#define IPC_HOME_PATH "/.smartfolder/"
#define IPC_RUN_PATH "/run/"
#define IPC_HOME_RUN_PATH IPC_HOME_PATH IPC_RUN_PATH

/**
 * Return the filename for the watch constructed from the folder.
 * The filename is constructed by simply replacing the '/' character by '_'
 * @param dst_path The destination path to construct the filename from
 * @param out_filename String to store the resulting filename
 */
void ipc_get_watch_name_from_des_path(char *dst_path, char * out_filename)
{
    strncpy(out_filename, dst_path, IPC_MAX_PATH);

    for (size_t i = 0; i < strlen(out_filename); ++i)
    {
        if (out_filename[i] == '/')
        {
            out_filename[i] = '_';
        }
    }
}

int ipc_set_watch(char *dst_path)
{
    // Get user home directory root
    char * home_dir = getenv("HOME");
    if (home_dir == NULL)
    {
        fprintf(stderr, "IPC: Error: Invalid user home directory");
        return 1;
    }

    // Create our run directory.
    // No check are done because if path already exists, nothing happen.
    char pid_path[IPC_MAX_PATH] = "";
    strncat(pid_path, home_dir, IPC_MAX_PATH);
    strncat(pid_path, IPC_HOME_PATH, IPC_MAX_PATH);
    mkdir(pid_path, S_IRWXU | S_IRWXG);

    strncat(pid_path, IPC_RUN_PATH, IPC_MAX_PATH);
    mkdir(pid_path, S_IRWXU | S_IRWXG);

    // Store the PID to a file named after the destination path
    char pid_filename[IPC_MAX_PATH] = "";
    ipc_get_watch_name_from_des_path(dst_path, pid_filename);
    strncat(pid_path, pid_filename, IPC_MAX_PATH);

    // Before writing file, check if the pid file exists
    struct stat buffer;
    if (stat(pid_path, &buffer) == 0)
    {
        fprintf(stderr, "IPC: Error: there already is a watch for '%s'", dst_path);
        return 1;
    }

    // Get our PID and write it to the constructed file
    int pid = getpid();

    FILE * pid_file = fopen(pid_path, "w");
    if (pid_file == NULL)
    {
        fprintf(stderr, "IPC: Error: cannot write pid to file '%s'\n", pid_path);
        perror("   ");
        return 1;
    }

    fprintf(pid_file, "%d", pid);
    fclose(pid_file);

    return 0;
}

int ipc_stop_watch(char *dst_path)
{
    (void)dst_path;

    // Get pid of the process that manage dst_path
    char pid_path[IPC_MAX_PATH] = "";
    char * home_dir = getenv("HOME");
    if (home_dir == NULL)
    {
        fprintf(stderr, "IPC: Error: Invalid user home directory");
        return 1;
    }
    strncat(pid_path, home_dir, IPC_MAX_PATH);
    strncat(pid_path, IPC_HOME_RUN_PATH, IPC_MAX_PATH);

    char pid_filename[IPC_MAX_PATH] = "";
    ipc_get_watch_name_from_des_path(dst_path, pid_filename);
    strncat(pid_path, pid_filename, IPC_MAX_PATH);

    FILE * pid_file = fopen(pid_path, "r");
    int pid = 0;
    if (pid_file == NULL)
    {
        fprintf(stderr, "IPC: Error: cannot get pid of instance for this path '%s'\n", dst_path);
        perror("   ");
        return 1;
    }

    fscanf(pid_file, "%d", &pid);
    fclose(pid_file);

    if (pid == 0)
    {
        fprintf(stderr, "IPC: Error: cannot get pid of instance for this path '%s'\n", dst_path);
        fprintf(stderr, "   : Invalid PID data in '%s'\n", pid_path);
        return 1;
    }

    // Remove pid file
    if (unlink(pid_path))
    {
        fprintf(stderr, "IPC: Error: cannot delete pid file '%s'\n", pid_path);
        perror("   ");
        return 1;
    }

    // Kill process SIGTERM
    if (kill(pid, SIGTERM))
    {
        fprintf(stderr, "IPC: Error: cannot kill process '%d'\n", pid);
        perror("   ");
        return 1;
    }

    return 0;
}
