/** Orchestrates the file search and creation of related symbolic links

    This is the main module.

    Once started it runs continously. It uses the `finder` module to get the files matching the `expression` within `search_path`.
    The resulting list is past to the `linker` module to update the `dst_path`
    It will wait a few seconds, and it will start over.

    @file
 */
 #ifndef SEARCHFOLDER_H
#define SEARCHFOLDER_H

#include "validator.h"


struct searchfolder_t;
/** Contains an instance of `searchfolder`.
    Can only be created by `searchfolder_create`
*/
typedef struct searchfolder_t searchfolder_t;

/** Creates a searchfolder

    @param dst_path The folder to be created with the symbolic links to the found files
    @param search_path The folder where to looks for files
    @param expression The expression to be matched by the files
    @returns The created searchfolder
*/
searchfolder_t *searchfolder_create(char *dst_path, char *search_path, parser_t *expression);

/** Starts a created searchfolder
    Once started, it will run until `searchfolder_stop` is called
    @see searchfolder_stop
    @param searchfolder The searchfolder to start
*/
void searchfolder_start(searchfolder_t *searchfolder);

/** Stops and frees the allocated resources folder assigned to a search
    @see searchfolder_create
    @param searchfolder The searchfolder to stop
*/
void searchfolder_stop(searchfolder_t *searchfolder);

#endif
