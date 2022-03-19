#pragma once
#include <stdbool.h>

typedef struct {
   const char *program_name;
   const char *adapter;
   unsigned short int conversion;
   bool help;
} options;

void usage(const char *program_name);
void help(const char *program_name);
options parse_args(char **argv);
