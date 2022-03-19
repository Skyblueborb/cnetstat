#pragma once
#include <stddef.h>
#include <sys/types.h>
#include <stdio.h>

typedef struct {
   /** Bytes recieved total **/
   size_t rxbytes;
   /** Bytes transmitted total **/
   size_t txbytes;

   /** Boot time in UNIX seconds **/
   time_t boottime;
   /** Bytes recieved since boot **/
   size_t rxbytes_boot;
   /** Bytes transmitted since boot **/
   size_t txbytes_boot;
} save;

save read_save();
void write_save(save);
