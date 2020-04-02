#include "include/skel.h"
#include "include/routing_table.h"
#include "include/arp.h"
#include "include/queue.h"
#include "include/icmp.h"

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

    if (routing_table[index].prefix != (destination & routing_table[index].subnet_mask))
        index = -1;

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

    /* Get the routing table */
    routing_table = read_from_file(&routing_table_length);
    arp_table = (arp_table_entry *) malloc(10 * sizeof(arp_table_entry));

    while (1) {
        rc = get_packet(&m);
        DIE(rc < 0, "get_message");

        char *source_ip = (char *) malloc(20 * sizeof(char)); /* IP of the sender */
        char *destination_ip = (char *) malloc(20 * sizeof(char)); /* IP of the receiver */
        uint8_t *interface_mac = (uint8_t *) malloc(6 * sizeof(uint8_t));

        /* Get the IP and the MAC of the interface from which the message came */
        char *interface_ip = get_interface_ip(m.interface);
        get_interface_mac(m.interface, interface_mac);

        /* Initialize the structures corresponding to the Ethernet and IP headers */
        struct ether_header *ethernet_header = (struct ether_header *) m.payload;
        struct iphdr *ip_header = (struct iphdr *) (m.payload + sizeof(struct ether_header));

        /* Check the type of the Ethernet header */
        if (ethernet_header->ether_type == htons(ETHERTYPE_IP)) {
            char *outgoing_interface_ip;
            routing_table_entry *best_route;

            /* Convert source and destination IP to string */
            inet_ntop(AF_INET, &(ip_header->daddr), destination_ip, 20);
            inet_ntop(AF_INET, &(ip_header->saddr), source_ip, 20);

            /* Check if the destination IP of the packet is a router interface; if the function
             * returns -1, the IP is not a user interface */
            int router_interface = is_router_interface(destination_ip);

            /* If the packet is corrupted, drop it */
            if (ip_checksum(ip_header, sizeof(struct iphdr)) != 0) {
                continue;
            }

            /* If the ttl field of the packet reached 1, send a time exceeded ICMP message to the
             * source (type 11) */
            if (ip_header->ttl <= 1) {
                packet time_exceeded;
                send_icmp_packet(&time_exceeded, router_interface, interface_ip, source_ip,
                                 ethernet_header->ether_shost, 11);
                send_packet(m.interface, &time_exceeded);
                continue;
            }

            /* Find the best route to the destination network and the IP of the interface of that
             * network */
            best_route = get_best_route(ip_header->daddr);

            /* If there is no route to the destination network, send a destination unreachable
             * ICMP packet (type 3) to the source host */
            if (best_route == NULL) {
                packet destination_unreachable;
                send_icmp_packet(&destination_unreachable, router_interface, interface_ip,
                                 source_ip, ethernet_header->ether_shost, 3);
                send_packet(m.interface, &destination_unreachable);
                continue;
            }

            /* If the message is an ECHO request destined to an interface of the router, answer
             * with an ECHO reply (of type 0) */
            if (ip_header->protocol == IPPROTO_ICMP && router_interface != -1) {
                packet echo_reply;
                send_icmp_packet(&echo_reply, router_interface, interface_ip, destination_ip,
                                 ethernet_header->ether_shost, 0);
                send_packet(m.interface, &echo_reply);
                continue;
            }

            /* Get the addresses of the interface connected to the destination host */
            outgoing_interface_ip = get_interface_ip(best_route->interface);

            uint8_t *outgoing_interface_mac = (uint8_t *) malloc(6 * sizeof(uint8_t));
            get_interface_mac(best_route->interface, outgoing_interface_mac);

            /* Find the entry containing the destination IP address in the ARP table */
            arp_table_entry *entry = get_arp_entry(arp_table, arp_table_length, destination_ip);

            /* If there is no entry containing the destination IP address, add the message in a
             * queue and send a broadcast ARP request to get the MAC of the host */
            if (entry == NULL) {
                packet request;
                send_arp_request(&request, outgoing_interface_mac, outgoing_interface_ip,
                                 destination_ip, best_route->interface);
                send_packet(best_route->interface, &request);
            } else {
                /* If the router knows the MAC of the destination, recalculate its fields and
                 * forward the packet */
                ip_header->ttl--;
                ip_header->check = 0;
                ip_header->check = ip_checksum(ip_header, sizeof(struct iphdr));

                build_ethernet_header(ethernet_header, interface_mac, entry->mac, ETHERTYPE_IP);
                send_packet(best_route->interface, &m);
            }

            free(outgoing_interface_mac);

        } else if (ethernet_header->ether_type == htons(ETHERTYPE_ARP)) {

            /* If the packet is an ARP frame, check its operation */
            /* Initialize an ARP header structure with the data provided in the packet */
            struct ether_arp *arp_header = (struct ether_arp *) (m.payload + sizeof(struct
                    ether_header));

            char *target_ip = (char *) malloc(20 * sizeof(char));

            /* Find the target IP of the ARP request and convert it to string */
            inet_ntop(AF_INET, &(arp_header->arp_tpa), target_ip, 20);
            inet_ntop(AF_INET, &(arp_header->arp_spa), source_ip, 20);

            /* If the packet is destined to the router, add the entry in the ARP table, then
             * check if the frame was an ARP request */
            if (!strcmp(target_ip, interface_ip)) {

                /* If the frame is a request, build a reply frame and send it out the interface
                 * it came from */
                if (arp_header->arp_op == htons(ARPOP_REQUEST)) {
                    packet reply;
                    send_arp_reply(&reply, arp_header, interface_mac, interface_ip, source_ip);
                    send_packet(m.interface, &reply);
                }

                /* Add the IP and MAC address of the source host to the ARP table */
                add_entry_to_table(arp_table, &arp_table_length, source_ip, arp_header->arp_sha);
            }

            free(target_ip);
        }

        /* Free the dynamically allocated variables */
        free(source_ip);
        free(destination_ip);
        free(interface_mac);
    }
}