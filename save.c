#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "save.h"
#include "utils.h"
#include "xdg.h"

save read_save() {
   char *confpath = xdg_config_dir("cnetstat");

   char tmp[strlen(confpath) + strlen("/saved") + 1];
   sprintf(tmp, "%s/saved", confpath);
   FILE *savef = fopen(tmp, "r");

   free(confpath);

   if(!savef) {
      save sv = {
         .rxbytes = 0,
         .txbytes = 0,
         .boottime = get_boot_time(),
         .rxbytes_boot = 0,
         .txbytes_boot = 0
      };
      return sv;
   }
   
   int ret;
   save sv;
   if((ret = fscanf(savef, "%zd %zd %zd %zd %zd", &sv.rxbytes, &sv.txbytes, &sv.boottime, &sv.rxbytes_boot, &sv.txbytes_boot)) != 5) {
      eprintf("Failed to read saved stats: %s\n", ret < 0 ? strerror(errno) : "Invalid format");
      exit(EXIT_FAILURE);
   }

   if(fclose(savef)) {
      perror("Failed to close save file");
      exit(EXIT_FAILURE);
   }
   return sv;
}
void write_save(save sv) {
   // FIXME: only call this once
   char* confpath = xdg_config_dir("cnetstat");
   if(mkdirp(confpath, 0777) < 0) {
      perror("Could not create config directory");
      exit(EXIT_FAILURE);
   }

   char tmp[strlen(confpath) + strlen("/saved") + 1];
   sprintf(tmp, "%s/saved", confpath);
   FILE* savef = fopen(tmp, "w+");

   if(!savef) {
      perror("Could not open save file");
      exit(EXIT_FAILURE);
   }

   free(confpath);

   int ret;
   if ((ret = fprintf(savef, "%zd %zd %zd %zd %zd", sv.rxbytes, sv.txbytes,
                     sv.boottime, sv.rxbytes_boot, sv.txbytes_boot)) < 0) {
      eprintf("Could not write to save file: %s\n", strerror(errno));
      exit(EXIT_FAILURE);
   }

   if(fclose(savef)) {
      perror("Failed to close save file");
      exit(EXIT_FAILURE);
   }
}
