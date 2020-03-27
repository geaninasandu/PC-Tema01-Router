#ifndef TEMA01_UTILS_H
#define TEMA01_UTILS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <netinet/if_ether.h>
#include "skel.h"

typedef struct {
    char *ip;
    uint8_t mac[6];
} arp_table_entry;

void build_arp_frame(struct ether_arp *arp_frame, uint16_t op, uint8_t *sha, char *spa, uint8_t
*tha, char *tpa);

void build_ethernet_header(struct ether_header *frame_header, uint8_t *source_mac, uint8_t
*destination_mac, uint16_t type);

void add_entry_to_table(arp_table_entry *arp_table, int *arp_table_length, char *ip_address,
                        uint8_t *mac_address);

arp_table_entry *get_arp_entry(arp_table_entry *arp_table, int arp_table_length, char
*destination_ip);

#endif //TEMA01_UTILS_H
