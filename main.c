#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void readBytes(FILE *fp, __uintmax_t* out ) {
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

int main()
{
   FILE *fprx = fopen("/sys/class/net/enp4s0/statistics/rx_bytes", "r");
   FILE *fptx = fopen("/sys/class/net/enp4s0/statistics/tx_bytes", "r");
   static __uintmax_t rxbytes;
   static __uintmax_t txbytes;
  
   readBytes(fprx, &rxbytes);
   readBytes(fptx, &txbytes);

   printf("Data downloaded: %lluMB\n",rxbytes/1000000);
   printf("Data uploaded: %lluMB\n",txbytes/1000000);
   printf("Raw bytes downloaded: %llu\n",rxbytes);
   printf("Raw bytes uploaded: %llu\n",txbytes);


   return 0;
}
