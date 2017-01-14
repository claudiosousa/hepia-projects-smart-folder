/**
 * Simple logger functions that allow logging of various informations and errors.
 * @author Claudio Sousa, Gonzalez David
 * @file
 */

#ifndef LOGGER_H
#define LOGGER_H

/**
 * Log a message on STDOUT if debug mode is enabled.
 * @param fmt Message to log with printf() format allowed
 * @param ... Format args
 */
void logger_debug(char * fmt, ...);
/**
 * Log a message on STDOUT.
 * @param fmt Message to log with printf() format allowed
 * @param ... Format args
 */
void logger_info(char * fmt, ...);
/**
 * Log an error message on STDERR.
 * @param fmt Message to log with printf() format allowed
 * @param ... Format args
 */
void logger_error(char * fmt, ...);
/**
 * Log an error message on STDERR using perror (POSIX)
 * @param msg Message to log
 */
void logger_perror(char * msg);

#endif
