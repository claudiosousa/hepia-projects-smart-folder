/**
 * Manage communication between different instance of smartfolder.
 * @author Claudio Sousa, Gonzalez David
 */

/**
 * Create the instance run file for futher identification by other instances
 * @param dst_path The destination path this instance operates on
 * @return Error indicator: 0 for OK, 1 for an error
 */
int ipc_set_watch(char *dst_path);

/**
 * Stop the instance designated by the given search path
 * @param dst_path The destination path that designate the instance
 * @return Error indicator: 0 for OK, 1 for an error
 */
int ipc_stop_watch(char *dst_path);
