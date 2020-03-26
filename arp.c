//
// Created by maria on 3/25/20.
//

#include "include/arp.h"

struct ether_arp *build_arp_frame(struct ether_arp *arp_frame, uint16_t op, uint8_t *sha, char
*spa, uint8_t *tha, char *tpa) {

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

    return arp_frame;
}

int mac_address_equal(uint8_t *mac1, uint8_t *mac2) {
    for (int i = 0; i < 6; ++i) {
        if (mac1[i] != mac2[i])
            return 0;
    }

    return 1;
}

int table_contains_entry(arp_table_entry *arp_table, int arp_table_length, uint8_t *mac_address) {

    for (int i = 0; i < arp_table_length; ++i) {
        if (mac_address_equal(arp_table[i].mac, mac_address))
            return 1;
    }

    return 0;
}

void add_entry_to_table(arp_table_entry *arp_table, int *arp_table_length, char *ip_address,
                        uint8_t *mac_address) {

    if (!table_contains_entry(arp_table, *arp_table_length, mac_address)) {

        inet_pton(AF_INET, ip_address, &(arp_table[*arp_table_length].ip));

        for (int i = 0; i < 6; ++i) {
            arp_table[*arp_table_length].mac[i] = mac_address[i];
        }

        (*arp_table_length)++;
    }

}


