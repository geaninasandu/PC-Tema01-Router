#include "include/routing_table.h"

/**
 * Comparator function used to sort the entries ascending by their prefix IP address;
 * If the prefixes are equal, sort them descending by the subnet mask, to get the best matching IP
 * as the first entry;
 * @param a     first entry;
 * @param b     second entry;
 * @return      1 if the entries should be interchanged; -1 otherwise;
 */
int entry_comparator(const void *a, const void *b) {
    const routing_table_entry *entry1 = (routing_table_entry *) a;
    const routing_table_entry *entry2 = (routing_table_entry *) b;

    if (entry1->prefix == entry2->prefix)
        return (entry1->subnet_mask > entry2->subnet_mask) ? -1 : 1;

    return (entry1->prefix < entry2->prefix) ? -1 : 1;
}

/**
 * Open the rtable file, read each line, then parse the entry and add it to an array of entries;
 * @param entry_number      passed by reference to get the number of entries upon call;
 * @return                  the sorted array of entries;
 */
routing_table_entry *read_from_file(int *entry_number) {
    FILE *file = fopen("rtable.txt", "r");
    char *routing_table_line = NULL;
    size_t line_length = 0, rtable_length = 100;
    ssize_t read;

    /* Dynamically allocate an array of routing_table_entry structs */
    routing_table_entry *rtable = (routing_table_entry *) malloc(rtable_length * sizeof
            (routing_table_entry));

    if (file == NULL) {
        exit(1);
    }

    while ((read = getline(&routing_table_line, &line_length, file)) != -1) {
        /* Parse the line and add a new entry in the routing table array */
        rtable[(*entry_number)] = parse_entry(routing_table_line);
        (*entry_number)++;

        /* If the number of entries exceeds the allocated size, reallocate the array */
        if ((*entry_number) == rtable_length) {
            rtable_length *= 10;
            rtable = (routing_table_entry *) realloc(rtable, rtable_length * sizeof
                    (routing_table_entry));
        }
    }

    /* Sort the array */
    qsort(rtable, (*entry_number), sizeof(routing_table_entry), entry_comparator);

    return rtable;
}

/**
 * Read each character of a line, adding it to the corresponding string, depending on the field;
 * The strings are then converted to uint32 to get the IP addresses;
 * @param routing_table_line    the line read from the file;
 * @return                      an entry object with all the members set;
 */
routing_table_entry parse_entry(char *routing_table_line) {
    routing_table_entry entry;
    char prefix_string[20], next_hop_string[20], subnet_mask_string[20], interface_string[2];
    int i = 0, j = 0;
    int field = 0;

    /* Traverse the line and add characters to the required strings */
    while (routing_table_line[i] != '\n' && routing_table_line[i]) {
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

        /* If the next character is ' ', increase the field and jump to the character after it;
         * Otherwise, go to the next character */
        if (routing_table_line[i + 1] == ' ') {
            i += 2;
            j = 0;
            field++;
        } else {
            i++;
            j++;
        }
    }

    /* Convert the strings to IP addresses of uint32 format */
    inet_pton(AF_INET, prefix_string, &(entry.prefix));
    inet_pton(AF_INET, next_hop_string, &(entry.next_hop));
    inet_pton(AF_INET, subnet_mask_string, &(entry.subnet_mask));
    entry.interface = atoi(interface_string);

    return entry;
}