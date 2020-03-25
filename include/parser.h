//
// Created by maria on 3/22/20.
//

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <arpa/inet.h>
<<<<<<< HEAD
#include "skel.h"
=======
>>>>>>> 05256a6b37c0e77f69e23220163c6970c222ace2

#ifndef PARSER_H
#define PARSER_H

typedef struct {
    uint32_t prefix;
    uint32_t next_hop;
    uint32_t subnet_mask;
    int interface;
} routing_table_entry;

<<<<<<< HEAD
typedef struct {
    uint32_t ip;
    uint8_t mac[6];
} arp_table_entry;

routing_table_entry *read_from_file();
arp_table_entry *parse_arp_table(int *entry_number);
arp_table_entry parse_arp_entry(char *arp_table_line);

routing_table_entry *read_from_file();

=======
routing_table_entry *read_from_file();
>>>>>>> 05256a6b37c0e77f69e23220163c6970c222ace2
routing_table_entry parse_entry(char *routing_table_line);

#endif
