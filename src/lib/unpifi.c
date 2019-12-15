/*
 * unpifi.c - unpifi
 *
 * Date   : 2019/12/12
 */
#include "unpifi.h"
#include <ifaddrs.h>

struct ifi_info *
get_ifi_info(int family, int doaliases)
{
    struct ifi_info *ifi, *ifihead, **ifipnext;
    int sockfd, flags;
    char lastname[IFNAMSIZ], *cptr;
    struct ifreq ifr;
    struct ifaddrs *ifap, *ifaptr;
    ssize_t addrlen = 0;

    sockfd = Socket(family, SOCK_DGRAM, 0);

    if (getifaddrs(&ifap) < 0) {
        err_sys("getifaddrs error");
    }

    ifihead = NULL;
    ifipnext = &ifihead;
    lastname[0] = 0;

    for(ifaptr = ifap; ifaptr != NULL; ifaptr = ifaptr->ifa_next) {
        if(ifaptr->ifa_addr->sa_family != family) {
            continue;
        }
        if ((cptr = strchr(ifaptr->ifa_name, ':')) != NULL) {
            *cptr = 0;
            if (strncmp(lastname, ifaptr->ifa_name, IFNAMSIZ) == 0) {
                if (doaliases == 0) {
                    continue;
                }
            }
        }
        memcpy(lastname, ifaptr->ifa_name, IFNAMSIZ);
        memcpy(ifr.ifr_name, ifaptr->ifa_name, IFNAMSIZ);

        flags = ifaptr->ifa_flags;
        ifi = Calloc(1, sizeof(struct ifi_info));
        *ifipnext = ifi;
        ifipnext = &ifi->ifi_next;
        ifi->ifi_flags = flags;

        memcpy(ifi->ifi_name, ifaptr->ifa_name, IFI_NAME);
        ifi->ifi_name[IFI_NAME-1] = 0;

        Ioctl(sockfd, SIOCGIFINDEX, &ifr);
        ifi->ifi_index = ifr.ifr_ifindex;


        switch (ifaptr->ifa_addr->sa_family) {
        case AF_INET:
            addrlen = sizeof(struct sockaddr_in);
            break;
        case AF_INET6:
            addrlen = sizeof(struct sockaddr_in6);
            break;
        default:
            addrlen = sizeof(struct sockaddr);
            break;
        }
        ifi->ifi_addr = Calloc(1, addrlen);
        memcpy(ifi->ifi_addr, ifaptr->ifa_addr, addrlen);
        if (flags & IFF_BROADCAST && ifaptr->ifa_broadaddr != NULL) {
            ifi->ifi_brdaddr = Calloc(1, addrlen);
            memcpy(ifi->ifi_brdaddr, ifaptr->ifa_broadaddr, addrlen);
        }
        if (flags & IFF_POINTOPOINT && ifaptr->ifa_dstaddr != NULL) {
            ifi->ifi_dstaddr = Calloc(1, addrlen);
            memcpy(ifi->ifi_dstaddr, ifaptr->ifa_dstaddr, addrlen);
        }
        if (ifaptr->ifa_netmask != NULL) {
            ifi->ifi_netmask = Calloc(1, addrlen);
            memcpy(ifi->ifi_netmask, ifaptr->ifa_netmask, addrlen);
        }

        Ioctl(sockfd, SIOCGIFMTU, &ifr);
        ifi->ifi_mtu = ifr.ifr_mtu;

        Ioctl(sockfd, SIOCGIFTXQLEN, &ifr);
        ifi->ifi_txqlen = ifr.ifr_qlen;

        Ioctl(sockfd, SIOCGIFMETRIC, &ifr);
        ifi->ifi_metric = ifr.ifr_metric;

        Ioctl(sockfd, SIOCGIFHWADDR, &ifr);
        ifi->ifi_hlen = 6;
        memcpy(ifi->ifi_haddr, ifr.ifr_hwaddr.sa_data, 6);

        Ioctl(sockfd, SIOCGIFMAP, &ifr);
        ifi->ifi_map = Calloc(1, sizeof(struct ifmap));
        memcpy(ifi->ifi_map, &ifr.ifr_map, sizeof(struct ifmap));
    }
    freeifaddrs(ifap);
    return(ifihead);
}

struct ifi_info *
Get_ifi_info(int family, int doaliases)
{
    struct ifi_info *n = NULL;
    if ((n = get_ifi_info(family, doaliases)) == NULL) {
        err_sys("get_ifi_info error");
    }
    return n;
}

void
free_ifi_info(struct ifi_info *ifihead)
{
    struct ifi_info *ifi, *ifinext;

    for (ifi = ifihead; ifi != NULL; ifi = ifinext) {
        if (ifi->ifi_addr != NULL) {
            free(ifi->ifi_addr);
        }
        if (ifi->ifi_brdaddr != NULL) {
            free(ifi->ifi_brdaddr);
        }
        if (ifi->ifi_dstaddr != NULL) {
            free(ifi->ifi_dstaddr);
        }
        if (ifi->ifi_netmask != NULL) {
            free(ifi->ifi_netmask);
        }
        if (ifi->ifi_map != NULL) {
            free(ifi->ifi_map);
        }
        ifinext = ifi->ifi_next;
        free(ifi);
    }
}
