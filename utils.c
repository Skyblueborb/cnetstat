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
#include <stddef.h>
#include <stdarg.h>
#include <ctype.h>

#include "utils.h"

char* get_user_name() {
    uid_t uid = geteuid();
    struct passwd *pw = getpwuid(uid);
    if (!pw)
        exit(EXIT_FAILURE);
    return pw->pw_name;
}

void perrorf(const char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    vfprintf(stderr, fmt, ap);
    va_end(ap);
    fprintf(stderr, ": %s\n", strerror(errno));
}

ssize_t getword(FILE* file, char** outptr) {
    int ret;

    while((ret = fgetc(file)) != EOF && isspace(ret))
        if(ret < 0)
            return -1;

    size_t len = 0;
    size_t cap = 64;
    char* word = malloc(cap);

    if(ret != EOF)
        word[len++] = ret;
    else {
        *outptr = word;
        return 0;
    }

    while((ret = fgetc(file)) != EOF) {
        if(ret < 0) {
            free(word);
            return -1;
        }
        if(isspace(ret))
            break;
        if(len >= cap - 1)
            word = realloc(word, cap *= 2);
        word[len++] = ret;
    }

    word[len] = 0;
    *outptr = word;
    return len;
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
    if(mkdir(path, mode) < 0 && errno != EEXIST)
            return -1;
    return 0;
}

char* find_interface() {
    DIR *dir = opendir("/sys/class/net");
    if (!dir) {
        perror("Failed to open /sys/class/net directory");
        exit(EXIT_FAILURE);
    }

    struct dirent *entry;
    while ((entry = readdir(dir))) {
        if(strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
            continue;

        // len("/sys/class/net/") = 15
        // len("/type") = 5
        char type_path[15 + strlen(entry->d_name) + 5 + 1];
        sprintf(type_path, "/sys/class/net/%s/type", entry->d_name);
        FILE *type_file = fopen(type_path, "r");
        if (!type_file) {
            perrorf("Failed to open %s", type_path);
            exit(EXIT_FAILURE);
        }

        char *type_str;
        if (getword(type_file, &type_str) < 0) {
            perrorf("Failed to read %s", type_path);
            exit(EXIT_FAILURE);
        }

        char* endptr;
        long type = strtol(type_str, &endptr, 10);
        if (endptr == type_str || *endptr != '\0') {
            perrorf("Failed to parse %s as number", type_path);
            exit(EXIT_FAILURE);
        }


        // Ignore virtual and loopback interfaces (hopefully this is right)
        // https://elixir.bootlin.com/linux/v5.18.3/source/include/uapi/linux/if_arp.h#L30
        if((type >= 768 && type <= 772)
           || (type >= 777 && type <= 779)
           || type == 783
        ) {
            free(type_str);
            fclose(type_file);
            continue;
        }
        fclose(type_file);
        free(type_str);

        // len("/sys/class/net/") = 15
        // len("/operstate") = 10
        char operstate_path[15 + strlen(entry->d_name) + 10 + 1];
        sprintf(operstate_path, "/sys/class/net/%s/operstate", entry->d_name);
        FILE *tmp = fopen(operstate_path, "r");
        if (!tmp) {
            perrorf("Failed to open %s", operstate_path);
            exit(EXIT_FAILURE);
        }

        char* word;
        if(getword(tmp, &word) < 0) {
            perrorf("Failed to read from %s", operstate_path);
            exit(EXIT_FAILURE);
        }

        if (strcmp(word, "up") == 0) {
            char* cpy = strdup(entry->d_name);
            closedir(dir);
            free(word);
            fclose(tmp);
            return cpy;
        }
        free(word);
        fclose(tmp);
    }

    closedir(dir);
    return NULL;
}

char *get_boot_id() {
    // len("/proc/sys/kernel/random/boot_id") = 31
    char random_boot_id_path[32] = "/proc/sys/kernel/random/boot_id";
    FILE *boot_id = fopen(random_boot_id_path, "r");
    if (!boot_id) {
        perrorf("Failed to open %s", random_boot_id_path);
        exit(EXIT_FAILURE);
    }

    char *word = NULL;
    ssize_t word_len = getword(boot_id, &word);
    if (word_len < 0) {
        perrorf("Failed to read from %s", random_boot_id_path);
        exit(EXIT_FAILURE);
    }

    fclose(boot_id);

    return word;
}
