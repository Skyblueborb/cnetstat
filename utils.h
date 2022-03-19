#pragma once
#include <sys/types.h>
#include <time.h>

#define eprintf(fmt, ...) fprintf(stderr, fmt, ##__VA_ARGS__)

char *get_user_name();

/**
 * Just like mkdir(2) but recursively creates parent directories.
 **/
int mkdirp(char *path, mode_t mode);

/**
 * Gets the time the system was booted at in seconds
 **/
time_t get_boot_time();
