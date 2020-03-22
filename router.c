#include "skel.h"
#include "include/parser.h"

routing_table_entry *rtable;
int rtable_length = 0;

int binary_search(int left, int right, uint32_t destination_ip) {

    // TODO: Daca crapa fmm timur

    if (right < left)
        return -1;

    int middle = left + (right - left) / 2;

    if (rtable[middle].prefix == destination_ip)
        return middle;

    if (rtable[middle].prefix > destination_ip)
        return binary_search(left, middle - 1, destination_ip);

    return binary_search(middle + 1, right, destination_ip);
}

routing_table_entry *get_best_route(uint32_t destination_ip) {
    int index = binary_search(0, rtable_length - 1, destination_ip);

    if (index == -1)
        return NULL;

    return &rtable[index];
}

int main(int argc, char *argv[]) {

    setvbuf(stdout, NULL, _IONBF, 0);
    packet m;
    int rc;

    init();

    while (1) {
        rc = get_packet(&m);
        DIE(rc < 0, "get_message");
        /* Students will write code here */

        struct ether_header *eth_hdr = (struct ether_header *) m.payload;
        struct iphdr *ip_hdr = (struct iphdr *) (m.payload + sizeof(struct ether_header));

        rtable = read_from_file(&rtable_length);

    }
}
