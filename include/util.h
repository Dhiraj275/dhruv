#pragma  once
#include <stdbool.h>
const char *get_mime_type(const char *path);
bool file_exists(char *path);
bool resolve_path(const char *path, char *out);


