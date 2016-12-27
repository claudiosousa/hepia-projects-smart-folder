#ifndef LINKER_H
#define LINKER_H

#include <stdlib.h>
#include "finder.h"

/**
 * Update the links in the destination folder:
 *   - create new link
 *   - delete old and invalid one
 * @param dst_path Where to put the links
 * @param files List of files to link
 */
void linker_update(char *dst_path, finder_t *files);

#endif
