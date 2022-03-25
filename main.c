#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <pwd.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "arg.h"
#include "save.h"
#include "utils.h"
#include "xdg.h"

int lenValue (uintmax_t value){
    int l=1;
    while(value>9){l++; value/=10;}
    return l;
}


char *toSensibleUnit(float bytes, int conversion) {
    const char *size_units[] = {"B", "kB", "MB", "GB", "TB", "PB", "EB"};

    char *buf = malloc(lenValue(bytes) + 3);
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
    buf = toSensibleUnit(tbytes, opts->conversion);
    printf("Data uploaded: %s\n", buf);
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
    FILE *rxf = fopen(path, "r");
    if(rxf == NULL) {
        eprintf("Could not open %s\n", path);
        exit(EXIT_FAILURE);
    }
    sprintf(path, "%s/statistics/tx_bytes", adapter_dir);
    FILE *txf = fopen(path, "r");
    if(txf == NULL) {
        eprintf("Could not open %s\n", path);
        exit(EXIT_FAILURE);
    }

    uintmax_t tmp;
    save sv = read_save();
    time_t boottime = get_boot_time();
    if(boottime != sv.boottime) {
       sv.rxbytes_boot = 0;
       sv.txbytes_boot = 0;
       sv.boottime = boottime;
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

    return 0;
}
