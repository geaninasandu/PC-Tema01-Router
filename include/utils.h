#ifndef TEMA01_UTILS_H
#define TEMA01_UTILS_H

#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>

typedef struct {
    uint16_t hrd;
    uint16_t pro;
    uint8_t hln;
    uint8_t pln;
    uint16_t op;
    uint8_t sha[6];
    uint32_t spa;
    uint8_t tha[6];
    uint32_t tpa;
} arp_hdr;

#endif //TEMA01_UTILS_H
