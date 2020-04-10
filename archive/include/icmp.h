#ifndef TEMA01_FORWARDING_H
#define TEMA01_FORWARDING_H

#include <stdlib.h>
#include "queue.h"
#include "skel.h"

int is_router_interface(char *destination_ip);

void build_icmp_header(struct iphdr *ip_header, struct icmphdr *icmp_header, char *source_ip,
                       char *destination_ip, int type, int sequence, int packet_size);

void send_icmp_packet(packet *echo_reply, int router_interface, char *interface_ip, char
*destination_ip, uint8_t *destination_mac, int type, int sequence);

#endif //TEMA01_FORWARDING_H
