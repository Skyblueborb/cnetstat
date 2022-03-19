#pragma once
#include <sys/types.h>

#define eprintf(fmt, ...) fprintf(stderr, fmt, ##__VA_ARGS__);

char *get_user_name();

/**
 * Just like mkdir(2) but recursively creates parent directories.
 **/
int mkdirp(char *path, mode_t mode);
