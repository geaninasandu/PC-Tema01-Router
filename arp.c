//
// Created by maria on 3/25/20.
//

#include "include/arp.h"

void build_arp_frame(struct ether_arp *arp_frame, uint16_t op, uint8_t *sha, char *spa, uint8_t
*tha, char *tpa) {

    arp_frame->arp_hrd = htons(ARPHRD_ETHER);
    arp_frame->arp_pro = htons(ETHERTYPE_IP);
    arp_frame->arp_hln = 6;
    arp_frame->arp_pln = 4;
    arp_frame->arp_op = htons(op);

    for (int i = 0; i < 6; ++i) {
        arp_frame->arp_tha[i] = tha[i];
        arp_frame->arp_sha[i] = sha[i];
    }

    inet_pton(AF_INET, spa, &(arp_frame->arp_spa));
    inet_pton(AF_INET, tpa, &(arp_frame->arp_tpa));
}

arp_table_entry *get_arp_entry(arp_table_entry *arp_table, int arp_table_length, char
*destination_ip) {

    for (int i = 0; i < arp_table_length; ++i) {
        if (!(strcmp(destination_ip, arp_table[i].ip)))
            return &arp_table[i];
    }

    return NULL;
}

void add_entry_to_table(arp_table_entry *arp_table, int *arp_table_length, char *ip_address,
                        uint8_t *mac_address) {

    if (get_arp_entry(arp_table, *arp_table_length, ip_address) == NULL) {

        strcpy(arp_table[*arp_table_length].ip, ip_address);

        for (int i = 0; i < 6; ++i) {
            arp_table[*arp_table_length].mac[i] = mac_address[i];
        }

        (*arp_table_length)++;
    }

}

void build_ethernet_header(struct ether_header *frame_header, uint8_t *source_mac, uint8_t
*destination_mac, uint16_t type) {

    for (int i = 0; i < 6; ++i) {
        frame_header->ether_dhost[i] = destination_mac[i];
        frame_header->ether_shost[i] = source_mac[i];
    }

    frame_header->ether_type = htons(type);
}