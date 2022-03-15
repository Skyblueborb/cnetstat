#include <stdio.h>
#include <limits.h>
#include <stdint.h>


int main()
{
    char ch, file_name[25];
    FILE *fp;
    static __uintmax_t rxbytes;
    char path[PATH_MAX];
    fp = fopen("/sys/class/net/enp4s0/statistics/rx_bytes", "r");
    while((ch = fgetc(fp)) != EOF)
      printf("%c", ch);

   fclose(fp);
}
