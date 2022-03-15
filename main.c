#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "arg.h"
#include "utils.h"

void readBytes(FILE *fp, uintmax_t *out) {
   char *ptr;
   char buffer[128];
   if (fp == NULL) {
      perror("Unable to open file");
      exit(EXIT_FAILURE);
   }
   fread(buffer, 128, 1, fp);
   fclose(fp);
   *out = strtoul(buffer, &ptr, 0);
   memset(buffer, 0, 128);
}

int main(int argc, char **argv) {
   options opt = parse_args(argv);
   if (opt.help) {
      help(opt.program_name);
      exit(EXIT_SUCCESS);
   }

   if (!opt.adapter) {
      eprintf("No network adapter provided\n");
      exit(EXIT_FAILURE);
   }

   const char *name = opt.adapter;
   // len("/sys/class/net/") = 15
   char adapter_dir[15 + strlen(name) + 1];
   sprintf(adapter_dir, "/sys/class/net/%s", name);

   // len("/sys/class/net/") = 15
   // len("/statistics/_x_bytes") = 20
   char path[15 + strlen(name) + 20 + 1];
   sprintf(path, "%s/statistics/rx_bytes", adapter_dir);
   FILE *fprx = fopen(path, "r");
   sprintf(path, "%s/statistics/tx_bytes", adapter_dir);
   FILE *fptx = fopen(path, "r");

   uintmax_t rxbytes;
   uintmax_t txbytes;

   readBytes(fprx, &rxbytes);
   readBytes(fptx, &txbytes);

   printf("Data downloaded: %luMB\n", rxbytes / 1000000);
   printf("Data uploaded: %luMB\n", txbytes / 1000000);
   printf("Raw bytes downloaded: %lu\n", rxbytes);
   printf("Raw bytes uploaded: %lu\n", txbytes);

   return 0;
}
