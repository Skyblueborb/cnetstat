#include <pwd.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "utils.h"

char *xdg_config_dir(char *postfix) {
  char *config_home;
  if ((config_home = getenv("XDG_CONFIG_HOME"))) {
    size_t config_home_len = strlen(config_home);
    char *buf = malloc(config_home_len + (postfix ? strlen(postfix) : 0) + 2);

    strcpy(buf, config_home);
    while (config_home[config_home_len - 1] == '/') {
      buf[--config_home_len] = 0;
    }
    if (postfix) {
      buf[config_home_len] = '/';
      strcpy(buf+config_home_len+1, postfix);
    }

    return buf;
  } else {
    char *name = get_user_name();
    size_t name_len = strlen(name);

    char *buf = malloc(strlen("/home/") + name_len + strlen("/.config/") +
                       (postfix ? strlen(postfix) : 0) + 1);

    strcpy(buf, "/home/");
    strcat(buf, name);
    strcat(buf, "/.config");
    if (postfix) {
      size_t end = strlen("/home/") + name_len + strlen("/.config");
      buf[end] = '/';
      strcpy(buf+end+1, postfix);
    }

    return buf;
  }
}
