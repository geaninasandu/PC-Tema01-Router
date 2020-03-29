//
// Created by maria on 3/28/20.
//

#include "include/forwarding.h"

void send_queued_messages(queue *q) {
//    for (int i = 0; i < q->size; ++i) {
//        //
//    }
}

/**
 * Check if the destination IP of the message is on of the router's interface;
 * @param destination_ip    the destination IP provided in the ip_header;
 * @return                  -1 if there is no corresponding interface; otherwise, the interface
 *                          number;
 */
int is_router_interface(char *destination_ip) {
    for (int i = 0; i < ROUTER_NUM_INTERFACES; ++i) {
        if (!strcmp(destination_ip, get_interface_ip(interfaces[i])))
            return i;
    }

    return -1;
}

int build_packet_header(struct iphdr *ip_header) {

    if (ip_checksum(ip_header, sizeof(struct iphdr)) != 0)
        return 0;

    if (ip_header->ttl < 1)
        return -1;

    ip_header->ttl--;
    ip_header->check = 0;
    ip_header->check = ip_checksum(ip_header, sizeof(struct iphdr));

    return 1;
}

void build_icmp_header(struct iphdr *ip_header, struct icmphdr *icmp_header, uint32_t source_ip,
        uint32_t destination_ip) {
    ip_header->version = 4;
    ip_header->ihl = sizeof(struct iphdr) / sizeof(uint32_t);
    ip_header->tos = 0;
    ip_header->tot_len = htons(sizeof(struct iphdr) + sizeof(struct icmphdr));
    ip_header->id = htons(1);
    ip_header->frag_off = htons(0);
    ip_header->ttl = 64;
    ip_header->protocol = IPPROTO_ICMP;
    ip_header->check = 0;
    ip_header->check = ip_checksum(ip_header, sizeof(struct iphdr));
    ip_header->saddr = source_ip;
    ip_header->daddr = destination_ip;

    icmp_header->code = 0;
    icmp_header->type = 8;
    icmp_header->checksum = 0;
    icmp_header->checksum = ip_checksum(icmp_header, sizeof(struct icmphdr));
    icmp_header->un.echo.id = htons(getpid());
}
