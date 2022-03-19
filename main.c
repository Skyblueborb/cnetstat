#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pwd.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "arg.h"
#include "utils.h"

int lenValue (uintmax_t value){
  int l=1;
  while(value>9){l++; value/=10;}
  return l;
}

const char *getUserName()
{
  register struct passwd *pw;
  register uid_t uid;

   uid = geteuid();
   pw = getpwuid(uid);
   if (!pw) exit(EXIT_FAILURE);
   return pw->pw_name;
}

void writeBytes(FILE *fprx, FILE *fptx, uintmax_t rbytes, uintmax_t tbytes) {
   char buffer[32];
   sprintf(buffer, "%ju", rbytes);
   fwrite(buffer, lenValue(rbytes), 1, fprx);
   sprintf(buffer, "%ju", tbytes);  
   fwrite(buffer, lenValue(tbytes), 1, fptx);
   // fclose(fprx);
   // fclose(fptx);
}

void readBytes(FILE *fprx, FILE *fptx, uintmax_t *out_rx, uintmax_t *out_tx) {
   char confpath[6 + strlen(getUserName()) + 18 + 1];
   sprintf(confpath, "/home/%s/.config/cnetstat/", getUserName());
   mkdir(confpath,0777);
   char bytepath[strlen(confpath) + 7];
   sprintf(bytepath, "%srxbytes", confpath);
   FILE *fpcfgrxread = fopen(bytepath, "r");
   FILE *fpcfgrxwrite = fopen(bytepath, "w");
   sprintf(bytepath, "%stxbytes", confpath);
   FILE *fpcfgtxread = fopen(bytepath, "r");
   FILE *fpcfgtxwrite = fopen(bytepath, "w");

   char *ptr;
   char buffer[32];
   uintmax_t rx = 0, tx = 0;
   uintmax_t rx_prev = 0, tx_prev = 0;
   fgets(buffer, 32, fprx);
   rx = strtoul(buffer, &ptr, 0);

   fgets(buffer, 32, fptx);
   tx = strtoul(buffer, &ptr, 0);
   // memset(buffer, 0, 32);
   
   char test[32];
   fgets(test,32,fpcfgrxread);
   
   rx_prev = strtoul(test, &ptr, 0);

   fgets(buffer,32,fpcfgtxread);
   tx_prev = strtoul(buffer, &ptr, 0);

   fprintf(stdout, "RX: %ju\n", rx_prev);
   fprintf(stdout, "TX: %ju\n", tx_prev);

   // if (rx_prev > rx) {
   //    rx = rx_prev;
   // } else if (tx_prev > tx) {
   //    tx = tx_prev;
   // }
   writeBytes(fpcfgrxwrite, fpcfgtxwrite, rx, tx);
   *out_rx = rx;
   *out_tx = tx;
   memset(buffer, 0, 32);
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
   FILE *fprx = fopen(path, "r");
   sprintf(path, "%s/statistics/tx_bytes", adapter_dir);
   FILE *fptx = fopen(path, "r");

   uintmax_t rxbytes;
   uintmax_t txbytes;


   readBytes(fprx, fptx, &rxbytes, &txbytes);
   printBytes(rxbytes, txbytes, &opt);

   return 0;
}
