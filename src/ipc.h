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

#ifndef IPC_H
#define IPC_H

typedef void (*ipc_stop_callback)(void*);

/**
 * Create the instance run file for further identification by other instances,
 * and setup signal catch.
 * @param dst_path The destination path this instance operates on
 * @param cb Callback when a stop signal is received
 * @param cb_arg Callback argument
 * @return Error indicator: 0 for OK, 1 for an error
 */
int ipc_set_watch(char *dst_path, ipc_stop_callback cb, void * cb_arg);

/**
 * Removes the watch file for the given search path.
 * @param dst_path The destination path that designate the instance
 * @return Error indicator: 0 for OK, 1 for an error
 */
int ipc_remove_watch(char *dst_path);

/**
 * Stop the instance designated by the given search path, using the SIGTERM signal.
 * @param dst_path The destination path that designate the instance
 * @return Error indicator: 0 for OK, 1 for an error
 */
int ipc_stop_watch(char *dst_path);

#endif
