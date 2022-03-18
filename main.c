#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

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

void printBytes(uintmax_t rbytes, uintmax_t tbytes, options *opts) {
   
   float rx_converted;
   float tx_converted;
   
   switch (opts->conversion) {
      case 0:
         printf("Data downloaded: %luMB\n", rbytes / 1000000);
         printf("Data uploaded: %luMB\n", tbytes / 1000000);
         break;
      case 1:
         printf("Data downloaded: %luKB\n", rbytes / 1000);
         printf("Data uploaded: %luKB\n", tbytes / 1000);
         break;
      case 2:
         rx_converted = (float)rbytes / 1000000000;
         tx_converted = (float)tbytes / 1000000000;
         printf("Data downloaded: %.2fGB\n", rx_converted);
         printf("Data uploaded: %.2fGB\n", tx_converted);
         break;
      default:
         printf("Raw bytes downloaded: %lu\n", rbytes);
         printf("Raw bytes uploaded: %lu\n", tbytes);
         break;
   }
   printf("Raw bytes downloaded: %lu\n", rbytes);
   printf("Raw bytes uploaded: %lu\n", tbytes);
}

int main(int argc, char **argv) {
   options opt = parse_args(argv);
   if (opt.help) {
      help(opt.program_name);
      exit(EXIT_SUCCESS);
   }

   if (!opt.adapter) {
      if(access("/sys/class/net/wlan0", F_OK ) == 0 ) {
         opt.adapter = "wlan0";
      } else if(access("/sys/class/net/eth0", F_OK ) == 0 ) {
         opt.adapter = "eth0";
      } else if(access("/sys/class/net/enp4s0", F_OK ) == 0 ) {
         opt.adapter = "enp4s0";
      } else {
      eprintf("No network adapter found/provided\n");
      exit(EXIT_FAILURE);
      }
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
   printBytes(rxbytes, txbytes, &opt);

   return 0;
}
