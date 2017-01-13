/**
 * Simple logger functions that allow logging of various informations and errors.
 * @author Claudio Sousa, Gonzalez David
 */

#include <stdio.h>
#include <unistd.h>
#include <stdarg.h>
#include <errno.h>
#include <string.h>
#include "logger.h"
#include "io.h"

#define LOGGER_MSG_MAX 256

void logger_debug(char * fmt, ...) {
    #ifdef DEBUG
    char msg[LOGGER_MSG_MAX] = "";

    va_list argptr;
    va_start(argptr, fmt);
    vsnprintf(msg, LOGGER_MSG_MAX, fmt, argptr);
    va_end(argptr);

    io_file_write_fd(STDOUT_FILENO, msg);
    #else
    (void)fmt;
    #endif
}

void logger_info(char * fmt, ...) {
    char msg[LOGGER_MSG_MAX] = "";

    va_list argptr;
    va_start(argptr, fmt);
    vsnprintf(msg, LOGGER_MSG_MAX, fmt, argptr);
    va_end(argptr);

    io_file_write_fd(STDOUT_FILENO, msg);
}

void logger_error(char * fmt, ...) {
    char msg[LOGGER_MSG_MAX] = "";

    va_list argptr;
    va_start(argptr, fmt);
    vsnprintf(msg, LOGGER_MSG_MAX, fmt, argptr);
    va_end(argptr);

    io_file_write_fd(STDERR_FILENO, msg);
}

void logger_perror(char * msg) {
    perror(msg);
}
