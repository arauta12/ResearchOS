#ifndef BPD_H
#define BPB_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

#include <fs/mbr.h>

static uint8_t sectPerCluster;
static uint8_t numFATs;
static int fatVersion;

void readBpb();


#endif
