/**
 * Helpers function around system calls for IO purpose
 * @author Claudio Sousa, Gonzalez David
 */

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include "io.h"

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
        perror("Open failed");
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
    (void)content;
    int content_size = strlen(content);

    int fd = open(path, O_WRONLY | O_CREAT, IO_DEFAULT_PERM);
    if (fd == -1) {
        perror("Open failed");
        return 1;
    }

    int write_size = write(fd, content, content_size);
    if (write_size != content_size) {
        perror("Write failed");
        return 1;
    }

    close(fd);

    return 0;
}

int io_file_delete(char *path) {
    if (unlink(path)) {
        perror("Unlink failed");
        return 1;
    }

    return 0;
}

bool io_directory_exists(char *path) {
    struct stat buffer;
    return (stat(path, &buffer) == 0) && S_ISDIR(buffer.st_mode);
}

int io_directory_create(char *path) {
    if (mkdir(path, IO_DEFAULT_MODE) != 0) {
        perror("Mkdir failed");
        return 1;
    }

    return 0;
}
