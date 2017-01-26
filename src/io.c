/**
 * Helpers function around system calls for IO purpose
 * @author Claudio Sousa, Gonzalez David
 * @file
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>
#include "io.h"
#include "logger.h"

/**
 * Buffer size for file content read
 */
#define IO_TMP_BUF_SIZE 4096
/**
 * Default file/directory permission on creation
 */
#define IO_DEFAULT_PERM 0644
/**
 * Default file/directory persmission on creation
 */
#define IO_DEFAULT_MODE S_IRWXU | S_IRWXG

bool io_file_exists(char *path) {
    struct stat buffer;
    return (stat(path, &buffer) == 0) && S_ISREG(buffer.st_mode);
}

int io_file_read_content(char *path, char *dst_buffer, size_t buf_size) {
    char tmp[IO_TMP_BUF_SIZE] = "";

    int fd = open(path, O_RDONLY);
    if (fd == -1) {
        logger_perror("IO: error: open failed");
        return 1;
    }

    while (read(fd, tmp, IO_TMP_BUF_SIZE) > 0) {
        strncat(dst_buffer, tmp, buf_size);
    }

    dst_buffer[buf_size - 1] = '\0';

    close(fd);

    return 0;
}

int io_file_write(char *path, char *content) {
    int fd = open(path, O_WRONLY | O_CREAT, IO_DEFAULT_PERM);
    if (fd == -1) {
        logger_perror("IO: error: open failed");
        return 1;
    }

    if (io_file_write_fd(fd, content) != 0) {
        close(fd);
        return 1;
    }

    close(fd);
    return 0;
}

int io_file_write_fd(int fd, char *content) {
    int content_size = strlen(content);

    int write_size = write(fd, content, content_size);
    if (write_size != content_size) {
        logger_perror("IO: error: write failed");
        return 1;
    }

    return 0;
}

int io_file_delete(char *path) {
    if (unlink(path)) {
        logger_perror("IO: error: unlink failed");
        return 1;
    }

    return 0;
}

bool io_directory_exists(char *path) {
    struct stat buffer;
    return (stat(path, &buffer) == 0) && S_ISDIR(buffer.st_mode);
}

io_file_list_t *io_directory_get_all(char *path) {
    io_file_list_t *filelist = NULL;
    DIR *dir = NULL;
    struct dirent *entry = NULL;

    dir = opendir(path);
    if (dir == NULL) {
        logger_perror("IO: error: open dir failed");
        return NULL;
    }

    while ((entry = readdir(dir)) != NULL) {
        if ((strncmp(entry->d_name, ".", 2) != 0) && (strncmp(entry->d_name, "..", 3) != 0)) {
            io_file_list_t *previousfile = filelist;
            filelist = malloc(sizeof(io_file_list_t));
            filelist->next = previousfile;
            filelist->file = malloc(sizeof(char) * (1 + strlen(entry->d_name)));
            strncpy(filelist->file, entry->d_name, 1 + strlen(entry->d_name));
        }
    }

    closedir(dir);

    return filelist;
}

void io_directory_get_all_free(io_file_list_t *filelist) {
    io_file_list_t *previous;
    while (filelist) {
        previous = filelist;
        filelist = filelist->next;
        free(previous->file);
        free(previous);
    }
}

int io_directory_create(char *path) {
    if (mkdir(path, IO_DEFAULT_MODE) != 0) {
        logger_perror("IO: error: mkdir failed");
        return 1;
    }

    return 0;
}

int io_directory_create_parent(char *path) {
    char path_progressive[IO_PATH_MAX_SIZE] = "";
    char *path_sep_pos = strchr(path, IO_PATH_SEP);

    while (path_sep_pos != NULL) {
        strncpy(path_progressive, path, path_sep_pos - path + 1);

        if (!io_directory_exists(path_progressive)) {
            if (io_directory_create(path_progressive) != 0) {
                return 1;
            }
        }

        path_sep_pos = strchr(path_sep_pos + 1, IO_PATH_SEP);
    }

    return 0;
}

int io_directory_delete(char *path) {
    char file_del[IO_PATH_MAX_SIZE] = "";
    io_file_list_t *files = io_directory_get_all(path);
    io_file_list_t *workingfile = files;
    while (workingfile) {
        sprintf(file_del, "%s%c%s", path, IO_PATH_SEP, workingfile->file);
        io_file_delete(file_del);
        workingfile = workingfile->next;
    }
    io_directory_get_all_free(files);

    if (rmdir(path) != 0) {
        logger_perror("IO: error: rmdir failed");
        return 1;
    }

    return 0;
}

bool io_link_exists(char *path) {
    struct stat buffer;
    return (lstat(path, &buffer) == 0) && S_ISLNK(buffer.st_mode);
}
