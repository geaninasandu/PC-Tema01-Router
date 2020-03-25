//
// Created by maria on 3/22/20.
//

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include "skel.h"

#ifndef PARSER_H
#define PARSER_H

typedef struct {
    uint32_t prefix;
    uint32_t next_hop;
    uint32_t subnet_mask;
    int interface;
} routing_table_entry;

typedef struct {
    uint32_t ip;
    uint8_t mac[6];
} arp_table_entry;

routing_table_entry *read_from_file();
arp_table_entry *parse_arp_table(int *entry_number);
arp_table_entry parse_arp_entry(char *arp_table_line);
routing_table_entry parse_entry(char *routing_table_line);

#endif
