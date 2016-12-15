/**
 * Manage communication between different instance of smartfolder.
 * @author Claudio Sousa, Gonzalez David
 */

#ifndef IPC_H
#define IPC_H

typedef void (*ipc_stop_callback)(void*);

/**
 * Create the instance run file for futher identification by other instances
 * @param dst_path The destination path this instance operates on
 * @param cb Callback when stop signal received
 * @param cb_arg Callback argument
 * @return Error indicator: 0 for OK, 1 for an error
 */
int ipc_set_watch(char *dst_path, ipc_stop_callback cb, void * cb_arg);

/**
 * Removes the watch file for the given search path
 * @param dst_path The destination path that designate the instance
 * @return Error indicator: 0 for OK, 1 for an error
 */
int ipc_remove_watch(char *dst_path);

/**
 * Stop the instance designated by the given search path
 * @param dst_path The destination path that designate the instance
 * @return Error indicator: 0 for OK, 1 for an error
 */
int ipc_stop_watch(char *dst_path);

#endif
