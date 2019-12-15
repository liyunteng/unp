/*
 * route.c - route
 *
 * Date   : 2019/12/15
 */
#include <stdio.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <linux/rtnetlink.h>
#include <net/if.h>
#include <stdlib.h>
#include <string.h>

#define BUFSIZE 4096

struct route_info {
    struct in_addr dst;
    struct in_addr src;
    struct in_addr gateway;
    int priority;
    char ifname[IF_NAMESIZE];
};

int read_nl_sock(int sockfd, char *buf, size_t bufsize, int seq, int pid)
{
    struct nlmsghdr *nlh;
    int readn = 0, msglen = 0;

    do {
        if ((readn = recv(sockfd, buf, bufsize - msglen, 0)) < 0) {
            perror("recv error");
            return -1;
        }
        nlh = (struct nlmsghdr *)buf;
        if (NLMSG_OK(nlh, (unsigned long)readn) == 0 || nlh->nlmsg_type == NLMSG_ERROR) {
            perror("recv packet error");
            return -1;
        }

        if (nlh->nlmsg_type == NLMSG_DONE) {
            break;
        } else {
            buf += readn;
            msglen += readn;
        }

        if ((nlh->nlmsg_flags & NLM_F_MULTI) == 0) {
            break;
        }
    } while (nlh->nlmsg_seq == (unsigned int)seq && nlh->nlmsg_pid == (unsigned int)pid);

    return msglen;
}

int parse_route(struct nlmsghdr *nlh, struct route_info *rt_info)
{
    struct rtmsg *rtmsg;
    struct rtattr *rtattr;
    int rtlen;
    struct in_addr addr;

    rtmsg = (struct rtmsg *)NLMSG_DATA(nlh);

    if (rtmsg->rtm_family != AF_INET || rtmsg->rtm_table != RT_TABLE_MAIN) {
        return -1;
    }

    rtattr = (struct rtattr *)RTM_RTA(rtmsg);
    rtlen = RTM_PAYLOAD(nlh);

    for (;RTA_OK(rtattr, rtlen); rtattr = RTA_NEXT(rtattr, rtlen)) {
        switch (rtattr->rta_type) {
        case RTA_OIF:
            if_indextoname(*(int *)RTA_DATA(rtattr),rt_info->ifname);
            break;
        case RTA_GATEWAY:
            rt_info->gateway.s_addr = *(u_int *)RTA_DATA(rtattr);
            break;
            /* case RTA_PREF: */
        case RTA_PREFSRC:
            addr.s_addr = *(u_int *)RTA_DATA(rtattr);
            printf("# prefsrc: %s\n", inet_ntoa(addr));
        case RTA_SRC:
            rt_info->src.s_addr = *(u_int*)RTA_DATA(rtattr);
            break;
        case RTA_DST:
            rt_info->dst.s_addr = *(u_int*)RTA_DATA(rtattr);
            break;
        case RTA_TABLE:
            printf("# table: %u\n", *(u_int*)RTA_DATA(rtattr));
            break;
        case RTA_PRIORITY:
            rt_info->priority = *(u_int*)RTA_DATA(rtattr);
            break;
        default:
            printf("# unknown type: %d\n", rtattr->rta_type);
            break;
        }
    }
    return 0;

}

int get_gateway()
{
    struct nlmsghdr *nlh;
    struct rtmsg *rtmsg;
    struct route_info *rt_info;
    char buf[BUFSIZE];
    int sock, len, msgseq = 0;

    if ((sock = socket(AF_NETLINK, SOCK_DGRAM, NETLINK_ROUTE)) < 0) {
        perror("socket error");
        return -1;
    }


    memset(buf, 0, BUFSIZE);
    nlh = (struct nlmsghdr *)buf;
    rtmsg = (struct rtmsg *)NLMSG_DATA(nlh);
    nlh->nlmsg_len = NLMSG_LENGTH(sizeof(struct rtmsg));
    nlh->nlmsg_type = RTM_GETROUTE;
    nlh->nlmsg_flags = NLM_F_DUMP | NLM_F_REQUEST;
    nlh->nlmsg_seq = msgseq++;
    nlh->nlmsg_pid = getpid();

    if (send(sock, nlh, nlh->nlmsg_len, 0) < 0) {
        perror("send error");
        return -1;
    }


    if ((len = read_nl_sock(sock, buf, BUFSIZE,  msgseq, getpid())) < 0) {
        printf("read nl sock failed\n");
        return -1;
    }

    rt_info = (struct route_info *) malloc(sizeof(struct route_info));

    for (; NLMSG_OK(nlh, (unsigned int)len); nlh = NLMSG_NEXT(nlh, len)) {
        memset(rt_info, 0, sizeof(struct route_info));
        if (parse_route(nlh, rt_info) == 0) {
            if (strcmp(inet_ntoa(rt_info->dst), "0.0.0.0") == 0) {
                printf("dst: default\n");
            } else {
                printf("dst: %s\n", inet_ntoa(rt_info->dst));
            }
            /* printf("src: %s\n", inet_ntoa(rt_info->src)); */
            printf("gw: %s\n", inet_ntoa(rt_info->gateway));
            printf("if: %s\n", rt_info->ifname);
            printf("priority: %d\n", rt_info->priority);
            printf("\n");
        }
    }


    free(rt_info);
    close(sock);
    return 0;
}

int main(void)
{
    get_gateway();
    return 0;
}
