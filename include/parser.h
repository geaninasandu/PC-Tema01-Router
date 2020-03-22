//
// Created by maria on 3/22/20.
//

#include <stdio.h>
#include <unistd.h>

#ifndef PARSER_H
#define PARSER_H

typedef struct {
    uint32_t prefix;
    uint32_t next_hop;
    uint32_t subnet_mask;
    int interface;
} routing_table_entry;


#endif
