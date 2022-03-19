#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <pwd.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "arg.h"
#include "save.h"
#include "utils.h"
#include "xdg.h"

int lenValue (uintmax_t value){
  int l=1;
  while(value>9){l++; value/=10;}
  return l;
}

void printBytes(uintmax_t rbytes, uintmax_t tbytes, options *opts) {
   float rx_converted;
   float tx_converted;
   
   switch (opts->conversion) {
      case 0:
         printf("Data downloaded: %juMB\n", rbytes / 1000000);
         printf("Data uploaded: %juMB\n", tbytes / 1000000);
         break;
      case 1:
         printf("Data downloaded: %juKB\n", rbytes / 1000);
         printf("Data uploaded: %juKB\n", tbytes / 1000);
         break;
      case 2:
         rx_converted = (float)rbytes / 1000000000;
         tx_converted = (float)tbytes / 1000000000;
         printf("Data downloaded: %.2fGB\n", rx_converted);
         printf("Data uploaded: %.2fGB\n", tx_converted);
         break;
      default:
         printf("Raw bytes downloaded: %ju\n", rbytes);
         printf("Raw bytes uploaded: %ju\n", tbytes);
         break;
   }
   printf("Raw bytes downloaded: %ju\n", rbytes);
   printf("Raw bytes uploaded: %ju\n", tbytes);
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
   FILE *rxf = fopen(path, "r");
   sprintf(path, "%s/statistics/tx_bytes", adapter_dir);
   FILE *txf = fopen(path, "r");

   uintmax_t tmp;

   save sv = read_save();
   time_t boottime = get_boot_time();
   if(boottime != sv.boottime) {
      sv.rxbytes_boot = 0;
      sv.txbytes_boot = 0;
   }

   int ret;
   if((ret = fscanf(rxf, "%zd", &tmp)) != 1) {
      eprintf("Could not read rx statistics: %s", ret < 0 ? strerror(errno) : "Invalid format");
      exit(EXIT_FAILURE);
   }
   sv.rxbytes += tmp - sv.rxbytes_boot;
   sv.rxbytes_boot = tmp;
   if((ret = fscanf(txf, "%zd", &tmp)) != 1) {
      eprintf("Could not read tx statistics: %s", ret < 0 ? strerror(errno) : "Invalid format");
      exit(EXIT_FAILURE);
   }
   sv.txbytes += tmp - sv.txbytes_boot;
   sv.txbytes_boot = tmp;

   printBytes(sv.rxbytes, sv.txbytes, &opt);
   write_save(sv);

   if(fclose(rxf)) {
      perror("Failed to close stat file");
      exit(EXIT_FAILURE);
   }
   if(fclose(txf)) {
      perror("Failed to close stat file");
      exit(EXIT_FAILURE);
   }


   return 0;
}
