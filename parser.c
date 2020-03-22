//
// Created by maria on 3/22/20.
//

#include "include/parser.h"

void read_from_file() {
    FILE *file = fopen("rtable.txt", "r");
    char *routing_table_line = NULL;
    size_t line_length = 0;
    ssize_t read;

    if (file == NULL) {
        exit(1);
    }

    while ((read = getline(&routing_table_line, &line_length, file)) != -1) {
        parseEntry(routing_table_line);
    }
}

routing_table_entry *parseEntry(char *routing_table_line) {
    routing_table_entry *entry;
    char prefix_string[20], next_hop_string[20], subnet_mask_string[20], interface_string[5];
    int interface;
    int i = 0;

    while (routing_table_line[i] != ' ') {
        ro
    }
}


