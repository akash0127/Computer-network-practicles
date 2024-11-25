#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/if_ether.h>
#include <net/if.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/if_packet.h>
#include <netinet/ether.h>

#define MAC_ADDR_LEN 6
#define IP_ADDR_LEN 4

// Function to get the MAC address from an ARP reply
void getMACFromARP(const unsigned char *packet) {
    struct ethhdr *eth_header = (struct ethhdr *)packet;
    struct ether_arp *arp_header = (struct ether_arp *)(packet + ETH_HLEN);

    // Check if the packet is an ARP reply
    if (ntohs(eth_header->h_proto) == ETH_P_ARP && ntohs(arp_header->ea_hdr.ar_op) == ARPOP_REPLY) {
        printf("MAC Address: ");
        for (int i = 0; i < MAC_ADDR_LEN; i++) {
            printf("%02x", arp_header->arp_sha[i]);
            if (i < MAC_ADDR_LEN - 1) printf(":");
        }
        printf("\n");
    }
}

// Function to send an ARP request
void sendARPRequest(int sockfd, const char *interface, const char *target_ip, const char *src_ip) {
    unsigned char buffer[ETH_FRAME_LEN];
    struct ethhdr *eth_header = (struct ethhdr *)buffer;
    struct ether_arp *arp_header = (struct ether_arp *)(buffer + ETH_HLEN);

    // Ethernet header
    memset(eth_header->h_dest, 0xff, MAC_ADDR_LEN); // Broadcast
    memset(eth_header->h_source, 0x00, MAC_ADDR_LEN); // Placeholder, will use the interface MAC later
    eth_header->h_proto = htons(ETH_P_ARP);

    // ARP header
    arp_header->ea_hdr.ar_hrd = htons(ARPHRD_ETHER);
    arp_header->ea_hdr.ar_pro = htons(ETH_P_IP);
    arp_header->ea_hdr.ar_hln = MAC_ADDR_LEN;
    arp_header->ea_hdr.ar_pln = IP_ADDR_LEN;
    arp_header->ea_hdr.ar_op = htons(ARPOP_REQUEST);

    // Fill ARP payload
    memset(arp_header->arp_tha, 0x00, MAC_ADDR_LEN); // Target hardware address (unknown)
    inet_pton(AF_INET, target_ip, arp_header->arp_tpa); // Target protocol address
    inet_pton(AF_INET, src_ip, arp_header->arp_spa);    // Sender protocol address

    // Get the MAC address of the interface
    struct ifreq ifr;
    strncpy(ifr.ifr_name, interface, IFNAMSIZ - 1);
    if (ioctl(sockfd, SIOCGIFHWADDR, &ifr) == -1) {
        perror("ioctl");
        exit(EXIT_FAILURE);
    }
    memcpy(eth_header->h_source, ifr.ifr_hwaddr.sa_data, MAC_ADDR_LEN);
    memcpy(arp_header->arp_sha, ifr.ifr_hwaddr.sa_data, MAC_ADDR_LEN);

    // Bind to the interface
    struct sockaddr_ll saddr = {0};
    saddr.sll_ifindex = if_nametoindex(interface);
    saddr.sll_halen = MAC_ADDR_LEN;
    memcpy(saddr.sll_addr, eth_header->h_dest, MAC_ADDR_LEN);

    // Send the ARP request
    if (sendto(sockfd, buffer, ETH_HLEN + sizeof(struct ether_arp), 0, (struct sockaddr *)&saddr, sizeof(saddr)) == -1) {
        perror("sendto");
        exit(EXIT_FAILURE);
    }
}

// Main function
int main() {
    const char *interface = "eth0";     // Network interface
    const char *target_ip = "192.168.1.1"; // Target IP address
    const char *src_ip = "192.168.1.2";    // Source IP address

    // Open a raw socket
    int sockfd = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
    if (sockfd == -1) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    // Send ARP request
    sendARPRequest(sockfd, interface, target_ip, src_ip);

    // Listen for ARP replies
    unsigned char buffer[ETH_FRAME_LEN];
    while (1) {
        ssize_t len = recvfrom(sockfd, buffer, ETH_FRAME_LEN, 0, NULL, NULL);
        if (len == -1) {
            perror("recvfrom");
            exit(EXIT_FAILURE);
        }

        // Extract MAC address from ARP reply
        getMACFromARP(buffer);
    }

    close(sockfd);
    return 0;
}
