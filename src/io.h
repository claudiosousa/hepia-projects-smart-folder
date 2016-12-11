/**
 * Helpers function around system calls for IO purpose
 * @author Claudio Sousa, Gonzalez David
 */

#ifndef IO_H
#define IO_H

#include <stdlib.h>
#include <stdbool.h>
#include <sys/stat.h>

#define IO_PATH_MAX_SIZE 4096
#define IO_DEFAULT_MODE S_IRWXU | S_IRWXG

/**
 * Determine if a file i the specified path exists
 * @param path File path to check
 * @return True if exists, false otherwise
 */
bool io_file_exists(char *path);

/**
 * Fully read a file and put it in the destination buffer
 * @param path File path to read
 * @param dst_buffer Buffer to put the result
 * @return Success result, return 0 only when it has fully read it, 1 if any error occurs
 */
int io_file_read_content(char *path, char *dst_buffer, size_t buf_size);

/**
 * Write the content in the specified path
 * @param path File path to read
 * @param content Content to put in the file
 * @return Success result, return 0 only when it has fully written it, 1 if any error occurs
 */
int io_file_write(char *path, char *content);

/**
 * Delete a file
 * @param path File path to delete
 * @return 0 if deleted, 1 if error
 */
int io_file_delete(char *path);

#endif