#pragma once
#include <sys/types.h>
#include <stdio.h>
#include <time.h>

#define eprintf(fmt, ...) fprintf(stderr, fmt, ##__VA_ARGS__)

char *get_user_name();

/**
 * Like mkdir(2) but recursively creates parent directories.
 **/
int mkdirp(char *path, mode_t mode);

/**
 * Like perror(3) but accepts a format string.
 */
void perrorf(const char *fmt, ...);

/**
 * Reads one word from a file.
 * A word is defined as a sequence of non-whitespace characters.
 * Whitespace is defined as characters for which isspace(3) returns true.
 *
 * @param file The file to read from.
 * @param wordptr A pointer to a char* that will be set to the word (has to be freed with free()).
 * @return Word length on success, -1 on error.
 */
ssize_t getword(FILE* file, char** wordptr);

/**
  * Returns the name of the first active interface.
  * Ignores loopback and virtual interfaces.
  **/
char* find_interface();

/**
 * Gets the random unique boot identifier.
 **/
char *get_boot_id();
