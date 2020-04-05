#include "include/arp.h"
#include "include/skel.h"

/**
 * Set the members of the ARP header structure;
 * @param arp_frame     The header of the ARP frame that should be sent;
 * @param op            The operation - ARP request / reply;
 * @param sha           Source MAC address;
 * @param spa           Source IP address;
 * @param tha           Target MAC address;
 * @param tpa           Target IP address;
 */
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

/**
 * Search for an IP address in the ARP table and return the corresponding entry;
 * @param arp_table             The array of ARP entries;
 * @param arp_table_length      The size of the array;
 * @param destination_ip        The IP address to be found;
 * @return                      The entry corresponding to the IP, otherwise NULL;
 */
arp_table_entry *get_arp_entry(arp_table_entry *arp_table, int arp_table_length, char
*destination_ip) {

    for (int i = 0; i < arp_table_length; ++i) {
        if (!(strcmp(destination_ip, arp_table[i].ip)))
            return &arp_table[i];
    }

    return NULL;
}

/**
 * Add the tuple of IP/MAC addresses to the ARP table;
 * @param arp_table             The array of ARP entries;
 * @param arp_table_length      The size of the array;
 * @param ip_address            The IP address of the host;
 * @param mac_address           The MAC address of the host;
 */
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

/**
 * Create the header of the Ethernet frame, setting its members;
 * @param frame_header          The Ethernet header;
 * @param source_mac            MAC address of the source;
 * @param destination_mac       MAC address of the destination;
 * @param type                  Type of the packet (ARP / IP);
 */
void build_ethernet_header(struct ether_header *frame_header, uint8_t *source_mac, uint8_t
*destination_mac, uint16_t type) {

    for (int i = 0; i < 6; ++i) {
        frame_header->ether_dhost[i] = destination_mac[i];
        frame_header->ether_shost[i] = source_mac[i];
    }

    frame_header->ether_type = htons(type);
}

/**
 * Set up the Ethernet and ARP headers of the packet corresponding to an ARP reply;
 * @param reply             The ARP reply packet;
 * @param arp_request       The header of the received ARP request;
 * @param interface_mac     The MAC address of the interface from which the packet came;
 * @param interface_ip      The IP address of the interface from which the packet came;
 * @param target_ip         The destination IP of the host who initiated the request;
 */
void send_arp_reply(packet *reply, struct ether_arp *arp_request, uint8_t *interface_mac, char
*interface_ip, char *target_ip) {

    /* Build the header of the ARP reply and the Ethernet frame */
    struct ether_arp *arp_reply = (struct ether_arp *) (reply->payload + sizeof(struct
            ether_header));
    build_arp_frame(arp_reply, ARPOP_REPLY, interface_mac, interface_ip, arp_request->arp_sha,
                    target_ip);

    struct ether_header *reply_eth_header = (struct ether_header *) reply->payload;
    build_ethernet_header(reply_eth_header, interface_mac, arp_reply->arp_tha, ETHERTYPE_ARP);
}

/**
 * Set up the Ethernet and ARP headers of the packet corresponding to an ARP request;
 * @param request                   The ARP request packet;
 * @param gateway_interface_mac     The MAC of the interface connected to the destination host;
 * @param gateway_interface_ip      The IP of the interface connected to the destination host;
 * @param destination_ip            The IP of the destination host;
 * @param interface                 The interface to the host;
 */
void send_arp_request(packet *request, uint8_t *gateway_interface_mac, char *gateway_interface_ip,
                      char *destination_ip, int interface) {

    /* Create the broadcast MAC address */
    uint8_t *broadcast_mac = (uint8_t *) malloc(6 * sizeof(uint8_t));
    for (int i = 0; i < 6; ++i) {
        broadcast_mac[i] = 0xff;
    }

    /* Build the request's packet headers and send the created packet */
    struct ether_arp *arp_request = (struct ether_arp *) (request->payload + sizeof
            (struct ether_header));
    build_arp_frame(arp_request, ARPOP_REQUEST, gateway_interface_mac, gateway_interface_ip,
                    broadcast_mac, destination_ip);

    struct ether_header *request_eth_header = (struct ether_header *) request->payload;
    build_ethernet_header(request_eth_header, gateway_interface_mac, broadcast_mac, ETHERTYPE_ARP);

    request->len = sizeof(struct ether_header) + sizeof(struct ether_arp);

    free(broadcast_mac);
}