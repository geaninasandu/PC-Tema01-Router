
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

routing_table_entry *read_from_file();
int entry_comparator(const void *a, const void *b);
routing_table_entry parse_entry(char *routing_table_line);


#endif