/*
 * printifinfo.c - printifinfo
 *
 * Date   : 2019/12/12
 */
#include "unp.h"

int main(int argc, char *argv[])
{
    struct ifi_info *ifi, *ifihead;
    struct sockaddr *sa;
    u_char *ptr;
    int i, family, doaliases;

    if (argc < 2 || argc > 3) {
        err_quit("Usage: %s <inet4|inet6> [<doaliases>]", argv[0]);
    }

    family = AF_INET;
    if (strcmp(argv[1], "inet4") == 0)
        family = AF_INET;
    else if (strcmp(argv[1], "inet6") == 0)
        family = AF_INET6;
    else
        err_quit("invalid <address-family>");

    doaliases = 1;
    if (argc == 3) {
        doaliases = atoi(argv[2]);
    }


    for (ifihead = ifi = Get_ifi_info(family, doaliases);
         ifi != NULL; ifi = ifi->ifi_next) {
        printf("%s: ", ifi->ifi_name);
        if (ifi->ifi_index != 0) {
            printf("(%d) ", ifi->ifi_index);
        }
        printf("<");
        if (ifi->ifi_flags & IFF_UP)        printf("UP ");
        if (ifi->ifi_flags & IFF_RUNNING)   printf("RUNNING ");
        if (ifi->ifi_flags & IFF_BROADCAST) printf("BCAST ");
        if (ifi->ifi_flags & IFF_DEBUG)     printf("DEBUG ");
        if (ifi->ifi_flags & IFF_MULTICAST) printf("MCAST ");
        if (ifi->ifi_flags & IFF_LOOPBACK)  printf("LOOP ");
        if (ifi->ifi_flags & IFF_POINTOPOINT) printf("P2P ");
        printf("\b>\n");


        if ((sa = ifi->ifi_addr) != NULL) {
            printf("  IP: %s\n", Sock_ntop(sa, sizeof(*sa)));
        }

        if ((i = ifi->ifi_hlen) > 0) {
            ptr = ifi->ifi_haddr;
            printf("  MAC: ");
            do {
                printf("%s%02X", (i == ifi->ifi_hlen) ? "" : ":", *(ptr++));
            } while (--i > 0);
            printf("\n");
        }

        if (ifi->ifi_mtu != 0) {
            printf("  MTU: %d\n", ifi->ifi_mtu);
        }
        if (ifi->ifi_txqlen != 0) {
            printf("  TXQLEN: %d\n", ifi->ifi_txqlen);
        }
        if (ifi->ifi_metric != 0) {
            printf("  METRIC: %d\n", ifi->ifi_metric);
        }
        if (ifi->ifi_map != NULL) {
            if (ifi->ifi_map->base_addr != 0) {
                printf("  BASE: %x\n", ifi->ifi_map->base_addr);
            }
            if (ifi->ifi_map->mem_start != 0) {
                printf("  MAP: 0x%lx - 0x%lx\n",
                       ifi->ifi_map->mem_start,
                       ifi->ifi_map->mem_end);
            }
            if (ifi->ifi_map->irq != 0) {
                printf("  IRQ: %u\n", ifi->ifi_map->irq);
            }
            if (ifi->ifi_map->dma != 0) {
                printf("  DMA: %u\n", ifi->ifi_map->dma);
            }
            if (ifi->ifi_map->port != 0) {
                printf("  PORT: %u\n", ifi->ifi_map->port);
            }
        }

        if ((sa = ifi->ifi_netmask) != NULL) {
            printf("  NETMASK: %s\n", Sock_ntop(sa, sizeof(*sa)));
        }

        if ((sa = ifi->ifi_brdaddr) != NULL) {
            printf("  BROADCAST: %s\n", Sock_ntop(sa, sizeof(*sa)));
        }

        if ((sa = ifi->ifi_dstaddr) != NULL) {
            printf("  DESTINATION: %s\n", Sock_ntop(sa, sizeof(*sa)));
        }
    }
    free_ifi_info(ifihead);
    return 0;
}
