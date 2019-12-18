/*
 * recvfromflags.c - recvfromflags
 *
 * Date   : 2019/12/18
 */

#include "unp.h"
#include <sys/param.h>          /* ALIGN macro for CMSG_NXTHDR() macro */

#undef MAXLINE
#define MAXLINE 20              /* for truncate msg */


ssize_t
recvfrom_flags(int fd, void *ptr, size_t nbytes, int *flagsp,
               SA *sa, socklen_t *salenptr, struct in_pktinfo *pktp)
{
    struct msghdr msg;
    struct iovec iov[1];
    ssize_t n;

    struct cmsghdr *cmptr;
    union {
        struct cmsghdr cm;
        char control[CMSG_SPACE(sizeof(struct in_pktinfo))];
    } control_un[4];

    msg.msg_control = control_un[0].control;
    msg.msg_controllen = sizeof(control_un) * 4;
    msg.msg_flags = 0;
    msg.msg_name = sa;
    msg.msg_namelen = *salenptr;

    iov[0].iov_base = ptr;
    iov[0].iov_len = nbytes;
    msg.msg_iov = iov;
    msg.msg_iovlen = 1;

    if ((n = recvmsg(fd, &msg, *flagsp)) < 0) {
        return (n);
    }
    *salenptr = msg.msg_namelen;
    if (pktp) {
        memset(pktp, 0, sizeof(*pktp));
    }
    *flagsp = msg.msg_flags;
    if (msg.msg_controllen < sizeof(struct cmsghdr) ||
        (msg.msg_flags & MSG_CTRUNC) || pktp == NULL)
        return n;

    for (cmptr = CMSG_FIRSTHDR(&msg); cmptr != NULL;
         cmptr = CMSG_NXTHDR(&msg, cmptr)) {
        if (cmptr->cmsg_level == SOL_IP &&
            cmptr->cmsg_type == IP_PKTINFO) {
            memcpy(pktp, CMSG_DATA(cmptr), sizeof(struct in_pktinfo));
            continue;
        }
        if (cmptr->cmsg_level == IPPROTO_IP &&
            cmptr->cmsg_type == IP_TOS)
        {
            printf("TOS: %u\n", *((unsigned char *)CMSG_DATA(cmptr)));
            continue;
        }
        if (cmptr->cmsg_level == IPPROTO_IP &&
            cmptr->cmsg_type == IP_TTL)
        {
            printf("TTL: %d\n", *((int *)CMSG_DATA(cmptr)));
            continue;
        }
        err_quit("unknown ancillary data, len = %d, level = %d, type = %d",
                 cmptr->cmsg_len, cmptr->cmsg_level, cmptr->cmsg_type);
    }
    return n;
}

void
dg_echo_recvfromflags(int sockfd, SA *pcliaddr, socklen_t clilen)
{
    int flags;
    const int on = 1;
    socklen_t len;
    ssize_t n;
    char msg[MAXLINE], str[INET6_ADDRSTRLEN], ifname[IFNAMSIZ];
    struct in_addr in_zero;
    struct in_pktinfo pktinfo;

    Setsockopt(sockfd,IPPROTO_IP,IP_PKTINFO, &on, sizeof(on));
    Setsockopt(sockfd,IPPROTO_IP,IP_RECVTOS, &on, sizeof(on));
    Setsockopt(sockfd,IPPROTO_IP,IP_RECVTTL, &on, sizeof(on));
    Setsockopt(sockfd,IPPROTO_IP,IP_RECVERR, &on, sizeof(on));

    memset(&in_zero, 0, sizeof(struct in_addr));

    for (;;) {
        len = clilen;
        flags = 0;
        n = recvfrom_flags(sockfd, msg, MAXLINE, &flags,
                           pcliaddr, &len, &pktinfo);
        printf("%ld-byte datagram from %s", n, Sock_ntop(pcliaddr, len));
        printf(", to %s(%s)", Inet_ntop(AF_INET, &pktinfo.ipi_addr, str, sizeof(str)),
               Inet_ntop(AF_INET, &pktinfo.ipi_spec_dst, str, sizeof(str)));

        if (pktinfo.ipi_ifindex > 0) {
            printf(", recv i/f = %s", if_indextoname(pktinfo.ipi_ifindex,ifname));
        }

        if (flags & MSG_EOR)
            printf(" (eor");
        if (flags & MSG_TRUNC)
            printf(" (datagram truncated)");
        if (flags & MSG_CTRUNC)
            printf(" (control info truncated)");
        if (flags & MSG_OOB)
            printf(" (oob)");
        if (flags & MSG_ERRQUEUE)
            printf(" (errqueue)");

        printf("\n");
        if (n > 0) {
            Sendto(sockfd, msg, n, 0, pcliaddr, len);
        } else {
            return;
        }

    }
}
