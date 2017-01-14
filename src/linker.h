/**
 * The linker has two jobs:
 *   - creates the links for the provided list of files found;
 *   - deletes old links that are no longer present in the list.
 *
 * In order to create the links, it simply take the base name of the target file
 * and create the link with this name in the destination folder.
 * But to manage duplicate, it count the number of files that has the same basename
 * in the list before the current one and if it is not 0, append the number to the name.
 *
 * To purge the destination folder, it gets all the links in it. Then, for each link,
 * it determines whether a file is no longer here by searching the target of the link in the list.
 * If it is not found, the linker deletes the link in the destination folder.
 *
 * @author Claudio Sousa, Gonzalez David
 */

#ifndef LINKER_H
#define LINKER_H

#include <stdlib.h>
#include "finder.h"

/**
 * Update the links in the destination folder (create new ones and purge older ones).
 * @param dst_path Where to put the links
 * @param files List of files to link
 */
void linker_update(char *dst_path, finder_t *files);

#endif
