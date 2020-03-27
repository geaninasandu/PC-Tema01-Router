#include "skel.h"
#include "include/routing_table.h"
#include "include/arp.h"

routing_table_entry *routing_table;
int routing_table_length = 0;

arp_table_entry *arp_table;
int arp_table_length = 0;

/**
 * Search for the destination IP address in the routing table, using a modified binary search
 * algorithm that returns the first occurrence of the IP (therefore the best matching route, due to
 * the sorting method);
 * @return      the index of the destination IP if found, otherwise -1;
 */
int binary_search(uint32_t destination) {

    int left = 0, right = routing_table_length - 1, middle = 0, index = -1;

    while (left <= right) {
        middle = (left + right) / 2;

        if (routing_table[middle].prefix >= (destination & routing_table[middle].subnet_mask)) {
            index = middle;
            right = middle - 1;
        } else {
            left = middle + 1;
        }
    }

    return index;
}

/**
 * Search for the best matching route to the destination;
 * @param destination_ip    the destination host address;
 * @return                  the routing table entry of the matching route;
 */
routing_table_entry *get_best_route(uint32_t destination_ip) {
    int index = binary_search(destination_ip);

    if (index == -1)
        return NULL;

    return &routing_table[index];
}

int main(int argc, char *argv[]) {
    setvbuf(stdout, NULL, _IONBF, 0);
    packet m;
    int rc;

    init();

    char *source_ip = (char *) malloc(20 * sizeof(char));
    char *target_ip = (char *) malloc(20 * sizeof(char));
    char *destination_ip = (char *) malloc(20 * sizeof(char));

    routing_table = read_from_file(&routing_table_length);
    arp_table = (arp_table_entry *) malloc(10 * sizeof(arp_table_entry));

    while (1) {
        rc = get_packet(&m);
        DIE(rc < 0, "get_message");

        char *destination_gateway;
        routing_table_entry *best_route;
        uint8_t *interface_mac = (uint8_t *) malloc(6 * sizeof(uint8_t));
        uint32_t interface_ip;

        /* Initialize the structures corresponding to the Ethernet and IP headers */
        struct ether_header *ethernet_header = (struct ether_header *) m.payload;
        struct iphdr *ip_header = (struct iphdr *) (m.payload + sizeof(struct ether_header));

        /* Get the IP and the MAC of the interface from which the message came */
        char *interface_ip_string = get_interface_ip(m.interface);
        get_interface_mac(m.interface, interface_mac);
        inet_pton(AF_INET, interface_ip_string, &(interface_ip));

        /* If the type of the ethernet header is 0x0806, the message is an ARP frame */
        if (ethernet_header->ether_type == htons(ETHERTYPE_ARP)) {

            /* Initialize an ARP header structure with the data provided in the packet */
            struct ether_arp *arp_header = (struct ether_arp *) (m.payload + sizeof(struct
                    ether_header));

            /* Find the target IP of the ARP request and convert it to string */
            inet_ntop(AF_INET, &(arp_header->arp_tpa), target_ip, 20);
            inet_ntop(AF_INET, &(arp_header->arp_spa), source_ip, 20);

            /* If the packet is destined to the router, add the entry in the ARP table, then
             * check if the frame was an ARP request */
            if (!strcmp(target_ip, interface_ip_string)) {

                /* If the frame is a request, build a reply frame and send it to the host */
                if (arp_header->arp_op == htons(ARPOP_REQUEST)) {
                    packet reply;

                    /* Build the header of the ARP reply and the Ethernet frame */
                    struct ether_arp *arp_reply = (struct ether_arp *) (reply.payload + sizeof
                            (struct ether_header));
                    build_arp_frame(arp_reply, ARPOP_REPLY, interface_mac, interface_ip_string,
                                    arp_header->arp_sha, source_ip);

                    struct ether_header *reply_eth_header = (struct ether_header *) reply.payload;
                    build_ethernet_header(reply_eth_header, interface_mac, arp_reply->arp_tha,
                                          ETHERTYPE_ARP);

                    /* Send the reply out the interface it came from */
                    send_packet(m.interface, &reply);
                }

                /* Add the IP and MAC address of the source host to the ARP table */
                add_entry_to_table(arp_table, &arp_table_length, source_ip, arp_header->arp_sha);
            }
        } else {
            /* If the message is not an ARP frame, check if the destination MAC address exists in
             * the ARP table */
            inet_ntop(AF_INET, &(ip_header->daddr), destination_ip, 20);

            /* Find the best route to the destination network and the IP of the interface of that
             * network */
            best_route = get_best_route(ip_header->daddr);
            destination_gateway = get_interface_ip(best_route->interface);

            /* If there is no entry containing the destination IP address, send a broadcast ARP
             * request to get the MAC of the host */
            if (get_arp_entry(arp_table, arp_table_length, destination_ip) == NULL) {
                packet request;
                uint8_t *broadcast_mac = (uint8_t *) malloc(6 * sizeof(uint8_t));

                for (int i = 0; i < 6; ++i) {
                    broadcast_mac[i] = 0xff;
                }

                /* Build the request's packet headers and send the created packet */
                struct ether_arp *arp_request = (struct ether_arp *) (request.payload + sizeof
                        (struct ether_header));
                build_arp_frame(arp_request, ARPOP_REQUEST, interface_mac, destination_gateway,
                                broadcast_mac, destination_ip);

                struct ether_header *request_eth_header = (struct ether_header *) request.payload;
                build_ethernet_header(request_eth_header, interface_mac, broadcast_mac,
                                      ETHERTYPE_ARP);

                send_packet(best_route->interface, &request);
            }
        }
    }
}


