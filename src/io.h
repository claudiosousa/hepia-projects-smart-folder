/**
 * Helpers function around system calls for IO purpose.
 * @author Claudio Sousa, Gonzalez David
 * @file
 */

#ifndef IO_H
#define IO_H

#include <stdlib.h>
#include <stdbool.h>
#include <sys/stat.h>

/**
 * Maximum string size for path
 */
#define IO_PATH_MAX_SIZE 4096
/**
 * Path separator for path construction
 */
#define IO_PATH_SEP '/'

/**
 * Container for file lists (fixed size for simplification),
 * used by io_directory_get_all().
 */
typedef struct io_file_list_t {
    /**
     * File path
     */
    char *file;
    /**
     * Next file in the chained list
     */
    struct io_file_list_t *next;
} io_file_list_t;

/**
 * Determine if a file in the specified path exists
 * @param path File path to check
 * @return True if exists, false otherwise
 */
bool io_file_exists(char *path);

/**
 * Fully read a file and put it in the destination buffer
 * @param path File path to read
 * @param dst_buffer Buffer to put the result
 * @param buf_size Size of the destination buffer (result)
 * @return Success result, return 0 only when it has fully read it, 1 if any error occurs
 */
int io_file_read_content(char *path, char *dst_buffer, size_t buf_size);

/**
 * Write the content in the specified path
 * @param path File path to write in
 * @param content Content to put in the file
 * @return Success result, return 0 only when it has fully written it, 1 if any error occurs
 */
int io_file_write(char *path, char *content);

/**
 * Write the content in the specified path
 * @param fd file descriptor to write in
 * @param content Content to put in the file descriptor
 * @return Success result, return 0 only when it has fully written it, 1 if any error occurs
 */
int io_file_write_fd(int fd, char *content);

/**
 * Delete a file
 * @param path File path to delete
 * @return 0 if deleted, 1 if error
 */
int io_file_delete(char *path);

/**
 * Determine if a directory in the specified path exists
 * @param path File path to check
 * @return True if exists, false otherwise
 */
bool io_directory_exists(char *path);

/**
 * Get all element contained in the specified directory
 * @param path Dir path to scan
 * @return List of files
 */
io_file_list_t *io_directory_get_all(char *path);

/**
 * Free a previously created list of files
 * @param filelist File list to free
 */
void io_directory_get_all_free(io_file_list_t *filelist);

/**
 * Create a directory in the specified path
 * @param path Dir path to create
 * @return 0 if created, 1 if error
 */
int io_directory_create(char *path);

/**
 * Create a directory in the specified path and all its parents.
 * If a the directory and its parents already exist, nothing is done.
 * @param path Dir path to create
 * @return 0 if created, 1 if error
 */
int io_directory_create_parent(char *path);

/**
 * Delete a directory and all childrens
 * @param path Directory path to delete
 * @return 0 if deleted, 1 if error
 */
int io_directory_delete(char *path);

/**
 * Determine if a link in the specified path exists
 * @param path Link path to check
 * @return True if exists, false otherwise
 */
bool io_link_exists(char *path);

#endif
