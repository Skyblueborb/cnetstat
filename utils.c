#include <pwd.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>

#include "utils.h"

char* get_user_name() {
   uid_t uid = geteuid();
   struct passwd *pw = getpwuid(uid);
   if (!pw)
      exit(EXIT_FAILURE);
   return pw->pw_name;
}

int mkdirp(char* path, mode_t mode) {
   for (char* ptr = strchr(path + 1, '/'); ptr; ptr = strchr(ptr + 1, '/')) {
      *ptr = '\0';

   if(mkdir(path, mode) < 0) {
      if(errno != EEXIST) {
         *ptr = '/';
         return -1;
      }
   }

   *ptr = '/';
  }
   if(mkdir(path, mode) < 0) {
      if(errno != EEXIST) {
         return -1;
      }
   }
   return 0;
}

// FIXME: THIS FUNCTION MAY NOT BE ACCURATE
time_t get_boot_time() {
   struct timespec tp;
   time_t t = time(NULL);
   if(clock_gettime(CLOCK_BOOTTIME, &tp) < 0) {
      perror("Could not get time from clock CLOCK_BOOTTIME");
      exit(EXIT_FAILURE);
   }

   return t - tp.tv_sec;
}
