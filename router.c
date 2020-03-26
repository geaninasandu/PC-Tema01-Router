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
//int binary_search(uint32_t destination_ip) {
//
//    int left = 0, right = routing_table_length - 1, middle, index = -1;
//
//    while (left <= right) {
//        middle = (left + right) / 2;
//        if (routing_table[middle].prefix >= destination_ip) {
//            index = middle;
//            right = middle - 1;
//        } else {
//            left = middle + 1;
//        }
//    }
//
//    return index;
//}
//
///**
// * Search for the best matching route to the destination;
// * @param destination_ip    the destination host address;
// * @return                  the routing table entry of the matching route;
// */
//routing_table_entry *get_best_route(uint32_t destination_ip) {
//    int index = binary_search(destination_ip);
//
//    if (index == -1)
//        return NULL;
//
//    return &routing_table[index];
//}

arp_table_entry *get_arp_entry(uint32_t destination_ip) {

    for (int i = 0; i < arp_table_length; ++i) {
        if (destination_ip == arp_table[i].ip)
            return &arp_table[i];
    }

    return NULL;
}

int main(int argc, char *argv[]) {
    setvbuf(stdout, NULL, _IONBF, 0);
    packet m;
    int rc;

    char *source_ip = (char *) malloc(20 * sizeof(char));
    char *target_ip = (char *) malloc(20 * sizeof(char));

    init();

    routing_table = read_from_file(&routing_table_length);
    arp_table = (arp_table_entry *) malloc(10 * sizeof(arp_table_entry));

    while (1) {
        rc = get_packet(&m);
        DIE(rc < 0, "get_message");
//
//        /* Initialize the structures corresponding to the Ethernet and IP headers */
        struct ether_header *ethernet_header = (struct ether_header *) m.payload;
        struct iphdr *ip_header = (struct iphdr *) (m.payload + sizeof(struct ether_header));


        uint8_t *interface_mac = (uint8_t *) malloc(6 * sizeof(uint8_t));
        uint32_t interface_ip;


        /* Get the IP and the MAC of the interface from which the message came */
        char *interface_ip_string = get_interface_ip(m.interface);
        get_interface_mac(m.interface, interface_mac);
        inet_pton(AF_INET, interface_ip_string, &(interface_ip));


        /* If the type of the ethernet header is 0x0806, the message is an ARP frame */
        if (ethernet_header->ether_type == htons(0x806)) {

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


                    struct ether_arp *arp_reply = (struct ether_arp *) (reply.payload + sizeof
                            (struct ether_header));


                    arp_reply = build_arp_frame(arp_reply, ARPOP_REPLY, interface_mac,
                                                interface_ip_string, arp_header->arp_sha,
                                                source_ip);

                    struct ether_header *reply_eth_header = (struct ether_header *) reply.payload;

                    for (int i = 0; i < 6; ++i) {
                        reply_eth_header->ether_dhost[i] = arp_reply->arp_tha[i];
                        reply_eth_header->ether_shost[i] = arp_reply->arp_sha[i];
                    }

                    reply_eth_header->ether_type = htons(0x806);

                    send_packet(m.interface, &reply);
                }

                add_entry_to_table(arp_table, &arp_table_length, source_ip, arp_header->arp_sha);
            }
        }
//
//        }
//
////        if (ip_checksum(ip_header, sizeof(struct iphdr)) != 0) {
////            printf("Bad checksum.\n");
////            continue;
////        }
////
////        if (ip_header->ttl < 1) {
////            printf("TTL exceeded.\n");
////            continue;
////        }
////
////        routing_table_entry *best_route = get_best_route(ip_header->daddr);
////        if (best_route == NULL)
////            printf("No route found.\n");
////
////        ip_header->ttl--;
////        ip_header->check = 0;
////        ip_header->check = ip_checksum(ip_header, sizeof(struct iphdr));
////
////        arp_table_entry *arp_entry = get_arp_entry(ip_header->daddr);
////        memcpy(ethernet_header->ether_dhost, arp_entry->mac, sizeof(arp_entry->mac));
////
////        send_packet(best_route->interface, &m);
//
//        free(interface_mac);
    }

    free(source_ip);
    free(target_ip);
    free(routing_table);
    free(arp_table);
}
