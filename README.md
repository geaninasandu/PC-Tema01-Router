# Simple Router

A simplified router firmware, developed by building and forwarding headers for protocols such as ARP, IP and ICMP.

## Routing table
- The routing table is represented by a static array of routing table entries.
- The routing table entries are sorted ascending by their prefix IP address, using quick sort.
- The best route is then determined by performing a binary search algorithm for the destination network.

## ARP
- The ARP messages are built using the *ether_arp* structure.
- Each tuple of IP - MAC address that reaches the router is stored in an ARP table.
- The router sends a broadcast ARP request when receiving a packet with an unknown destination MAC address, then stores the necessary entry in the ARP table after receiving the ARP reply.

## IP
- The IP headers are build using the *iphdr* struct.
- Upon receiving a packet, the router computes a checksum to make sure it is not corrupted. Corrupted packets are dropped.
- The router then checks the TTL field of the packet. If the field reached 0, the router sends an ICMP Time Exceeded response to the source.
- If the routing table contains no route to the destination network, the router sends an ICMP Destination Unreachable response.
- If all the verifications have passed, the packed is then forwarded to the next hop.
