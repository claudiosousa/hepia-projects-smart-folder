/**
   The finder retrives the list of files matching an expression

   A specified is search resursively for all find valid against a give filtering expression.

   Symbolic links are followed, and an hashtable is kept with the id's (inode id) of the found files to avoid
   processing the same paths twice and avoid infinte loops.

   @file
 */

#ifndef FINDER_H
#define FINDER_H

#include <stdlib.h>
#include "validator.h"

/** A chained list of found file names */
typedef struct finder_t {
    char *filename; /**< Found file name */
    struct finder_t *next; /**< Next in the chain */
} finder_t;

/** Finds the files in the `search_path` matchin the `expression`

    Search is performed recusively and symbolic links are followed.

    @param search_path Where to look for the files
    @param expression Filter expression used against found files
    @returns List of found files
 */
finder_t *finder_find(char *search_path, parser_t *expression);

/** Frees the memory allocated by `finder`
    @param  finder The instance to be freed
 */
void finder_free(finder_t *finder);

#endif
