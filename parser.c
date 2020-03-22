//
// Created by maria on 3/22/20.
//

#include "include/parser.h"

int entry_comparator(const void *a, const void *b) {
    const routing_table_entry *entry1 = (routing_table_entry *) a;
    const routing_table_entry *entry2 = (routing_table_entry *) b;

    if (entry1->prefix == entry2->prefix)
        return (entry1->subnet_mask > entry2->subnet_mask) ? -1 : 1;

    return (entry1->prefix < entry2->prefix) ? -1 : 1;
}

routing_table_entry *read_from_file(int *entry_number) {

    FILE *file = fopen("rtable.txt", "r");
    char *routing_table_line = NULL;
    size_t line_length = 0, rtable_length = 100;
    ssize_t read;

    routing_table_entry *rtable = (routing_table_entry *) malloc(sizeof(routing_table_entry) * rtable_length);

    if (file == NULL) {
        exit(1);
    }

    while ((read = getline(&routing_table_line, &line_length, file)) != -1) {
        rtable[(*entry_number)] = parse_entry(routing_table_line);
        (*entry_number)++;

        if ((*entry_number) == rtable_length) {
            rtable_length *= 10;
            rtable = (routing_table_entry *) realloc(rtable, rtable_length * sizeof(routing_table_entry));
        }
    }

    qsort(rtable, (*entry_number), sizeof(routing_table_entry), entry_comparator);

    return rtable;
}

routing_table_entry parse_entry(char *routing_table_line) {
    routing_table_entry entry;
    char prefix_string[20], next_hop_string[20], subnet_mask_string[20], interface_string[5];
    int interface;
    int i = 0, j = 0;
    int field = 0;

    while (routing_table_line[i]) {

        switch (field) {
            case 0:
                prefix_string[j] = routing_table_line[i];
                prefix_string[j + 1] = '\0';
                break;

            case 1:
                next_hop_string[j] = routing_table_line[i];
                next_hop_string[j + 1] = '\0';
                break;

            case 2:
                subnet_mask_string[j] = routing_table_line[i];
                subnet_mask_string[j + 1] = '\0';
                break;

            case 3:
                interface_string[j] = routing_table_line[i];
                interface_string[j + 1] = '\0';
                break;

            default:
                break;
        }

        if (routing_table_line[i + 1] == ' ') {
            i += 2;
            j = 0;
            field++;
        } else {
            i++;
            j++;
        }
    }

    char str[30];

    inet_pton(AF_INET, prefix_string, &(entry.prefix));
    inet_pton(AF_INET, next_hop_string, &(entry.next_hop));
    inet_pton(AF_INET, subnet_mask_string, &(entry.subnet_mask));
    entry.interface = atoi(interface_string);

    return entry;
}


