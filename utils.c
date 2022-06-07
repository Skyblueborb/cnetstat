#define _GNU_SOURCE

#include <pwd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/sysinfo.h>
#include <linux/sysinfo.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <stdio.h>

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

char* find_adapter() {
    DIR *dir = opendir("/sys/class/net");
    if (!dir) {
        perror("Failed to open /sys/class/net directory");
        exit(EXIT_FAILURE);
    }

    struct dirent *entry;
    while ((entry = readdir(dir))) {
        char *buf;
        asprintf(&buf, "/sys/class/net/%s/device", entry->d_name);
        if (access(buf, F_OK) == 0) {
            free(buf);
            free(dir);
            return entry->d_name;
        }
    }

    free(dir);
    eprintf("No network adapter found/provided\n");
    exit(EXIT_FAILURE);
}

time_t get_boot_time() {
    struct sysinfo inf;
    sysinfo(&inf);
    long uptime=inf.uptime;
    time_t t = time(NULL);
    return t - uptime;
}
