#ifndef SEARCHFOLDER_H
#define SEARCHFOLDER_H

#include "validator.h"

struct searchfolder_t;
typedef struct searchfolder_t searchfolder_t;

searchfolder_t *searchfolder_create(char *dst_path, char *search_path, parser_t *expression);
void searchfolder_start(searchfolder_t *searchfolder);
void searchfolder_stop(searchfolder_t *searchfolder);

#endif
