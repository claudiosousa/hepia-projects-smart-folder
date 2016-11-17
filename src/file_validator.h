struct file_validator_t;

typedef struct file_validator_t file_validator_t;

file_validator_t *file_validator_create(char *expression[], size_t size);
bool file_validator_validate(char *filename, file_validator_t *validator);