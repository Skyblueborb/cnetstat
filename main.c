#include <stdio.h>
#include <limits.h>
#include <stdint.h>


int main()
{
    char ch;
    FILE *fp;
    char txt[127];
    int iter = 0;
    int fcontent;
    static __uintmax_t rxbytes;
    fp = fopen("/sys/class/net/enp4s0/statistics/rx_bytes", "r");
   //  long size = fsize(fp);
   //  fcontent = malloc(size);
   //  fread(fcontent, 1, size, fp);
   while((ch = getc(fp)) != EOF) {
    txt[iter] = putchar(ch);
    iter++;
}
   //  while((ch = fgetc(fp)) != EOF)
      // txt[iter] = ch;
      // printf("%c", ch);
      // iter++;
   // txt[0] = fgetc(fp);
   fclose(fp);
   printf(txt);
   // rxbytes = atoll(txt);
   // printf("%llu", rxbytes);
}
