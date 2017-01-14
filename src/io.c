/**
 * Helpers function around system calls for IO purpose
 * @author Claudio Sousa, Gonzalez David
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>
#include "io.h"
#include "logger.h"

#define IO_TMP_BUF_SIZE 4096
#define IO_DEFAULT_PERM 0644
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

// TODO: use code of finder for recursive
io_file_list * io_directory_get_all(char *path) {
    io_file_list *filelist = NULL;
    DIR *dir = NULL;
    struct dirent *entry = NULL;

    dir = opendir(path);
    if (dir == NULL) {
        logger_perror("IO: error: open dir failed");
        return NULL;
    }

    filelist = malloc(sizeof(io_file_list));
    if (filelist == NULL) {
        logger_perror("IO: error: file_list malloc failed");
        return NULL;
    }
    filelist->count = 0;

    while ((entry = readdir(dir)) != NULL) {
        if ((strncmp(entry->d_name, ".", 2) != 0) && (strncmp(entry->d_name, "..", 3) != 0)) {
            strncpy(filelist->files[filelist->count++], entry->d_name, IO_PATH_MAX_SIZE);
        }
    }

    closedir(dir);

    return filelist;
}

void io_directory_get_all_free(io_file_list *filelist) {
    free(filelist);
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
    char * path_sep_pos = strchr(path, IO_PATH_SEP);

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
    // TODO: use code from finder for recursive
    char file_del[IO_PATH_MAX_SIZE] = "";
    io_file_list * files = io_directory_get_all(path);
    for (unsigned int i = 0; i < files->count; i++) {
        sprintf(file_del, "%s%c%s", path, IO_PATH_SEP, files->files[i]);
        io_file_delete(file_del);
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
