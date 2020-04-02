#include "include/icmp.h"
#include "include/arp.h"

/**
 * Check if the destination IP of the message is on of the router's interface;
 * @param destination_ip    the destination IP provided in the ip_header;
 * @return                  -1 if there is no corresponding interface; otherwise, the interface
 *                          number;
 */
int is_router_interface(char *destination_ip) {

    for (int i = 0; i < ROUTER_NUM_INTERFACES; ++i) {
        if (!strcmp(destination_ip, get_interface_ip(i)))
            return i;
    }

    return -1;
}

/**
 * Set the fields of the IP and ICMP headers when sending an ICMP packet;
 * @param ip_header         The IP header of the new ICMP packet;
 * @param icmp_header       The ICMP header;
 * @param source_ip         The IP address of the router interface connected to the host;
 * @param destination_ip    The IP of the host that initiated the ICMP packet exchange;
 * @param type              The type of the ICMP message (0 - ECHO reply; 3 - Destination
 * unreachable; 11 - Time exceeded);
 */
void build_icmp_header(struct iphdr *ip_header, struct icmphdr *icmp_header, char *source_ip,
                       char *destination_ip, int type) {

    ip_header->version = 4;
    ip_header->ihl = 5;
    ip_header->tos = 0;
    ip_header->tot_len = htons(sizeof(struct iphdr) + sizeof(struct icmphdr));
    ip_header->id = htons(1);
    ip_header->frag_off = htons(0);
    ip_header->ttl = 64;
    ip_header->protocol = IPPROTO_ICMP;
    ip_header->check = 0;
    ip_header->check = ip_checksum(ip_header, sizeof(struct iphdr));
    inet_pton(AF_INET, source_ip, &(ip_header->saddr));
    inet_pton(AF_INET, destination_ip, &(ip_header->daddr));

    icmp_header->code = 0;
    icmp_header->type = type;
    icmp_header->checksum = 0;
    icmp_header->checksum = ip_checksum(icmp_header, sizeof(struct icmphdr));
    icmp_header->un.echo.id = htons(getpid());
}

/**
 * Initialize and build the IP and ICMP header of the new ICMP packet the router should send;
 * @param icmp_message          The packet containing the ICMP message;
 * @param router_interface      The interface connected to the destination host;
 * @param interface_ip          The IP of the router interface;
 * @param destination_ip        The IP address of the receiving host;
 * @param destination_mac       The MAC address of the destination host;
 * @param type                  The type of the ICMP message (0 - ECHO reply; 3 - Destination
 * unreachable; 11 - Time exceeded);
 */
void send_icmp_packet(packet *icmp_message, int router_interface, char *interface_ip, char
*destination_ip, uint8_t *destination_mac, int type) {

    struct iphdr *ip_reply = (struct iphdr *) (icmp_message->payload + sizeof(struct ether_header));

    struct icmphdr *icmp_reply = (struct icmphdr *) (icmp_message->payload + sizeof(struct
            ether_header) + sizeof(struct iphdr));
    build_icmp_header(ip_reply, icmp_reply, interface_ip, destination_ip, type);

    /* Get the MAC address of the interface that received the ping */
    uint8_t *pinged_interface_mac = malloc(6 * sizeof(uint8_t));
    get_interface_mac(router_interface, pinged_interface_mac);

    struct ether_header *ethernet_reply = (struct ether_header *) icmp_message->payload;
    build_ethernet_header(ethernet_reply, pinged_interface_mac, destination_mac, ETHERTYPE_IP);
}