//
// Created by maria on 3/28/20.
//

#ifndef TEMA01_FORWARDING_H
#define TEMA01_FORWARDING_H

#include <stdlib.h>
#include "queue.h"
#include "skel.h"

int is_router_interface(char *destination_ip);

void build_icmp_header(struct iphdr *ip_header, struct icmphdr *icmp_header, uint32_t source_ip,
                       uint32_t destination_ip);


#endif //TEMA01_FORWARDING_H
