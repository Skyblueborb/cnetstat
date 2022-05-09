#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "arg.h"
#include "utils.h"

void usage(const char *program_name) {
    printf("USAGE:\n");
    printf("  %s [--help, --gb ...] <network_device>\n", program_name);
}
void help(const char *program_name) {
    usage(program_name);
    printf("\n");
    printf("OPTIONS:\n");
    printf("  -h, --help    display this help message\n");
    printf("  -b, --bytes   prints raw bytes in addition to conversion\n");
    printf("  -k, --kb      converts the output to kilobytes\n");
    printf("  -m, --mb      converts the output to megabytes\n");
    printf("  -g, --gb      converts the output to gigabytes\n");
    printf("  -t, --tb      converts the output to terabytes\n");
}

void parse_positional(char *positional, options *opts) {
    if (!opts->adapter)
       opts->adapter = positional;
    else {
       eprintf("Unexpected positional argument %s\n", positional);
       exit(EXIT_FAILURE);
    }
}

options parse_args(char **argv) {
    options opt = {.help = false, .program_name = NULL, .adapter = NULL, .conversion = 0,
                  /*.save_file = NULL, .read_file = NULL*/};
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
            // -b
            case 'b':
               opt.raw = true;
               break;
            // -k
            case 'k':
               opt.conversion = 1;
               break;
            case 'm':
               opt.conversion = 2;
               break;
            // -g
            case 'g':
               opt.conversion = 3;
               break;
            // -t
            case 't':
               opt.conversion = 4;
               break;
            // case 's':
            //    opt.save_file = *++argv;
            //    break;
            // case 'r':
            //    opt.read_file = *++argv;
            //    break;
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
       } else if (strcmp(*argv, "--tb") == 0) {
          opt.conversion = 4;
       } else if (strcmp(*argv, "--gb") == 0) {
          opt.conversion = 3;
       } else if (strcmp(*argv, "--mb") == 0) {
          opt.conversion = 2;
       } else if (strcmp(*argv, "--kb") == 0) {
          opt.conversion = 1;
       } else if (strcmp(*argv, "--bytes") == 0) {
          opt.raw = true;
       }
          /*else if (strcmp(*argv, "--save-file") == 0) {
          opt.save_file = *++argv;
       } else if (strcmp(*argv, "--read-file") == 0) {
          opt.read_file = *++argv;
       } */else if (strcmp(*argv, "--") == 0) {
          // If we encounter a "--" we stop parsing and treat further arguments
          // as non-flag arguemnts
          ++argv;
          break;
       } else {
          parse_positional(*argv, &opt);
       }
    }
    while (*argv) {
       parse_positional(*argv++, &opt);
    }

    return opt;
}
