#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <pwd.h>
#include <math.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "arg.h"
#include "save.h"
#include "utils.h"
#include "xdg.h"

int lenValue (uintmax_t value) {
    // log10 seems to be a little faster than repeatedly dividing by 10
    return (value != 0) * (int)(log10(value)) + 1;
}


char *toSensibleUnit(float bytes, int conversion) {
    const char *size_units[] = {"B", "kB", "MB", "GB", "TB", "PB", "EB"};

    char* buf = malloc(lenValue(bytes) + 6);
    int unit_index = 0;
    while (conversion == 0 && bytes > 1000 && unit_index < 6) {
        bytes /= 1000;
        unit_index++;
    }
    while(conversion != 0 && conversion > unit_index) {
        bytes /= 1000;
        unit_index++;
    }
    sprintf(buf, "%.2f%s", bytes, size_units[unit_index]);

    return buf;
}


void printBytes(uintmax_t rbytes, uintmax_t tbytes, options *opts) {
    char *buf= NULL;
    // printf("Conversion level: %hu\n", opts->conversion);
    buf = toSensibleUnit(rbytes, opts->conversion);
    printf("Data downloaded: %s\n", buf);
    free(buf);
    buf = toSensibleUnit(tbytes, opts->conversion);
    printf("Data uploaded: %s\n", buf);
    free(buf);
    if(opts->raw) {
        printf("Raw bytes downloaded: %lu\n", rbytes);
        printf("Raw bytes uploaded: %lu\n", tbytes);
    }
}

int main(int argc, char **argv) {
    options opt = parse_args(argv);
    if (opt.help) {
       help(opt.program_name);
       exit(EXIT_SUCCESS);
    }

    bool interface_free = false;
    if (opt.interface == NULL) {
        opt.interface = find_interface();
        if(opt.interface == NULL) {
            fprintf(stderr, "Could not find active interface\n");
            exit(EXIT_FAILURE);
        }
        interface_free = true;
    }

    const char *name = opt.interface;
    // len("/sys/class/net/") = 15
    char interface_dir[15 + strlen(name) + 1];
    sprintf(interface_dir, "/sys/class/net/%s", name);

    // len("/sys/class/net/") = 15
    // len("/statistics/_x_bytes") = 20
    char path[15 + strlen(name) + 20 + 1];
    sprintf(path, "%s/statistics/rx_bytes", interface_dir);
    FILE *rxf = fopen(path, "r");
    if(rxf == NULL) {
        eprintf("Could not open %s\n", path);
        exit(EXIT_FAILURE);
    }
    sprintf(path, "%s/statistics/tx_bytes", interface_dir);
    FILE *txf = fopen(path, "r");
    if(txf == NULL) {
        eprintf("Could not open %s\n", path);
        exit(EXIT_FAILURE);
    }

    uintmax_t tmp;
    save sv = read_save(opt.wipe);
    char* boot_id = get_boot_id();
    bool free_sv_boot_id = true;
    if(strcmp(boot_id, sv.boot_id)) {
       free(sv.boot_id);
       sv.rxbytes_boot = 0;
       sv.txbytes_boot = 0;
       sv.boot_id = boot_id;
       free_sv_boot_id = false;
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

    if(interface_free)
        free((void*)opt.interface);

    if(free_sv_boot_id)
        free(sv.boot_id);

    free(boot_id);

    return 0;
}
