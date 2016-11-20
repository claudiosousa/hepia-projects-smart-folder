#ifndef SMART_FOLDER_H
#define SMART_FOLDER_H

#include "file_validator.h"
struct smart_folder_t;
typedef struct smart_folder_t smart_folder_t;

smart_folder_t *smart_folder_create(char *dst_path, char *search_path, file_validator_t *validator);
void smart_folder_start(smart_folder_t *smart_folder);
void smart_folder_stop(smart_folder_t *smart_folder);

#endif