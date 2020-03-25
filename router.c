#include "skel.h"
#include "include/parser.h"
#include "include/utils.h"

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
int binary_search(uint32_t destination_ip) {

    int left = 0, right = routing_table_length - 1, middle, index = -1;

    while (left <= right) {
        middle = (left + right) / 2;
        if (routing_table[middle].prefix >= destination_ip) {
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

    init();

    routing_table = read_from_file(&routing_table_length);
    arp_table = parse_arp_table(&arp_table_length);

    while (1) {
        rc = get_packet(&m);
        DIE(rc < 0, "get_message");
        /* Students will write code here */

        struct ether_header *ethernet_header = (struct ether_header *) m.payload;
        struct iphdr *ip_header = (struct iphdr *) (m.payload + sizeof(struct ether_header));

        if(ethernet_header->ether_type == htons(0x806)) {

            arp_hdr *arp_header = (arp_hdr *) (m.payload + sizeof(struct ether_header));

//            if (arp_header->op == htons(ARPOP_REQUEST)) {
//
//
////                printf("%s", )
//            }

        }
        printf("\n");

//        if (ip_checksum(ip_header, sizeof(struct iphdr)) != 0) {
//            printf("Bad checksum.\n");
//            continue;
//        }
//
//        if (ip_header->ttl < 1) {
//            printf("TTL exceeded.\n");
//            continue;
//        }
//
//        routing_table_entry *best_route = get_best_route(ip_header->daddr);
//        if (best_route == NULL)
//            printf("No route found.\n");
//
//        ip_header->ttl--;
//        ip_header->check = 0;
//        ip_header->check = ip_checksum(ip_header, sizeof(struct iphdr));
//
//        arp_table_entry *arp_entry = get_arp_entry(ip_header->daddr);
//        memcpy(ethernet_header->ether_dhost, arp_entry->mac, sizeof(arp_entry->mac));
//
//        send_packet(best_route->interface, &m);


        struct ether_arp *arp = (struct ether_arp *) (m.payload + sizeof(struct ether_header));

        /* If the number of the ether_type field of the ethernet header is 0x0806, the message is
         * an ARP packet */
        if (ethernet_header->ether_type == htons(0x806)) {
//            printf("%u %u %d %d %u %s %hhn\n", arp->arp_hrd, arp->arp_pro, arp->arp_hln,
//                    arp->arp_pln, arp->arp_op, arp->arp_sha, arp->arp_spa);
        printf("%d\n", arp->arp_spa);
//            printf("%s\n", m.payload);
//            printf("%d\n", ethernet_header->ether_shost);
        } else if (ethernet_header->ether_type == htons(0x800)) {
            printf("2");
        }


    }
}
