#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "arg.h"
#include "utils.h"

void usage(const char *program_name) {
   printf("USAGE:\n");
   printf("  %s [--help] <network_device>\n", program_name);
}
void help(const char *program_name) {
   usage(program_name);
   printf("\n");
   printf("OPTIONS:\n");
   printf("  -h, --help    display this help message\n");
}

options parse_args(char **argv) {
   options opt = {.help = false, .program_name = NULL, .adapter = NULL};
   if (*argv == NULL) {
      eprintf("Running this program without argv[0] is unsupported!\n");
      exit(EXIT_FAILURE);
   }
   opt.program_name = *argv;

   while (*++argv) {
      // Short options
      // This parser supports chaining options like -abcd!
      if (**argv == '-' && (*argv)[1] != '-') {
         const char *short_options = *argv;

         while (*++short_options) {
            switch (*short_options) {
            // -h
            case 'h':
               opt.help = true;
               break;
            default:
               eprintf("Unknown argument -%c\n", *short_options);
               exit(EXIT_FAILURE);
            }
         }

         continue;
      }

      // Long options
      if (strcmp(*argv, "--help") == 0) {
         opt.help = true;
      } else if (strcmp(*argv, "--") == 0) {
         // If we encounter a "--" we stop parsing and treat further arguments
         // as non-flag arguemnts
         ++argv;
         break;
      } else {
         if (!opt.adapter)
            opt.adapter = *argv;
         else {
            eprintf("Unexpected positional argument %s\n", *argv);
            exit(EXIT_FAILURE);
         }
      }
   }
   while (*argv) {
      // FIXME: Reduce code duplication here!
      if (!opt.adapter)
         opt.adapter = *argv;
      else {
         eprintf("Unexpected positional argument %s\n", *argv);
         exit(EXIT_FAILURE);
      }
   }

   return opt;
}
