/*
 * unpifi.h - unpifi
 *
 * Date   : 2019/12/12
 */
#ifndef UNPIFI_H
#define UNPIFI_H

#include "unp.h"
#include <net/if.h>

#define IFI_NAME    16  /* same as IFNAMSIZ in <net/if.h> */
#define IFI_HADDR   8

struct ifi_info {
    char ifi_name[IFI_NAME];    /* interface name, null-terminated */
    short ifi_index;            /* interface index */
    int ifi_mtu;                /* interface MTU */
    u_char ifi_haddr[IFI_HADDR]; /* hardware address */
    u_short ifi_hlen;            /* # bytes in hardware address: 0, 6, 8 */
    int ifi_flags;              /* IFF_xxx constants from <net/if.h> */
    short ifi_myflags;           /* our own IFI_xxx flags */
    struct sockaddr *ifi_addr;   /* primary address */
    struct sockaddr *ifi_brdaddr; /* broadcast address */
    struct sockaddr *ifi_dstaddr; /* destination address */
    struct ifi_info *ifi_next;    /* next of these struct */
};

#define IFI_ALIAS 1             /* ifi_addr is an alias */

struct ifi_info *get_ifi_info(int family, int doaliases);
struct ifi_info *Get_ifi_info(int family, int doaliases);
void free_ifi_info(struct ifi_info *ifi);

#endif
