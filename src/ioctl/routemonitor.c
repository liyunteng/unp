/*
 * route_monitor.c - route monitor
 *
 * Date   : 2019/12/15
 */
#include <arpa/inet.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <linux/netlink.h>
#include <linux/rtnetlink.h>
#include <linux/route.h>
#include <sys/epoll.h>

#define EPOLL_LISTEN_MAX_CNT 256
#define EPOLL_LISTEN_TIMEOUT 512

int g_nlfd = -1;
int g_epollfd = -1;

void parse_rtattr(struct rtattr **tb, int max, struct rtattr *attr, int len)
{
    for (; RTA_OK(attr, len); attr = RTA_NEXT(attr, len)) {
        if (attr->rta_type <= max)
            tb[attr->rta_type] = attr;
    }
}

void nl_netroute_handle(struct nlmsghdr *nlh)
{
    int len;
    struct rtattr *tb[RTA_MAX + 1];
    struct rtmsg *rt;
    char tmp[256];

    memset(tb, 0, sizeof(tb));
    rt = NLMSG_DATA(nlh);
    len = nlh->nlmsg_len - NLMSG_SPACE(sizeof(*rt));
    parse_rtattr(tb, RTA_MAX, RTM_RTA(rt), len);
    if (tb[RTA_DST] != NULL) {
        inet_ntop(rt->rtm_family, RTA_DATA(tb[RTA_DST]), tmp, sizeof(tmp));
        printf("DST: %s", tmp);
    }
    if (tb[RTA_SRC] != NULL) {
        inet_ntop(rt->rtm_family, RTA_DATA(tb[RTA_SRC]), tmp, sizeof(tmp));
        printf("SRC: %s", tmp);
    }
    if (tb[RTA_GATEWAY] != NULL) {
        inet_ntop(rt->rtm_family, RTA_DATA(tb[RTA_GATEWAY]), tmp, sizeof(tmp));
        printf("GATEWAY: %s", tmp);
    }
    printf("\n");
}

void nl_netifinfo_handle(struct nlmsghdr *nlh)
{
    int len;
    struct rtattr *tb[IFLA_MAX + 1];
    struct ifinfomsg *ifinfo;

    memset(tb, 0, sizeof(tb));
    ifinfo = NLMSG_DATA(nlh);
    len = nlh->nlmsg_len - NLMSG_SPACE(sizeof(*ifinfo));
    parse_rtattr(tb, IFLA_MAX, IFLA_RTA(ifinfo), len);

    printf("%s: %s ", (nlh->nlmsg_type == RTM_NEWLINK) ? "NEWLINK" : "DELLINK", (ifinfo->ifi_flags & IFF_UP) ? "UP" : "DOWN");
    if (tb[IFLA_IFNAME]) {
        printf("%s", (char *)RTA_DATA(tb[IFLA_IFNAME]));
    }
    printf("\n");
}

void nl_netifaddr_handle(struct nlmsghdr *nlh)
{
    int len;
    struct rtattr *tb[IFA_MAX + 1];
    struct ifaddrmsg *ifaddr;
    char tmp[256];

    memset(tb, 0, sizeof(tb));
    ifaddr = NLMSG_DATA(nlh);
    len = nlh->nlmsg_len - NLMSG_SPACE(sizeof(*ifaddr));
    parse_rtattr(tb, IFA_MAX, IFA_RTA(ifaddr), len);

    printf("%s ", (nlh->nlmsg_type == RTM_NEWADDR) ? "NEWADDR" : "DELADDR");
    if (tb[IFA_LABEL] != NULL) {
        printf("%s ", (char *)RTA_DATA(tb[IFA_LABEL]));
    }
    if (tb[IFA_ADDRESS] != NULL) {
        inet_ntop(ifaddr->ifa_family, RTA_DATA(tb[IFA_ADDRESS]), tmp, sizeof(tmp));
        printf("%s ", tmp);
    }
    printf("\n");
}

void nl_netlink_hanlde(int fd)
{
    int r_size;
    socklen_t len = 0;
    char buf[2048] = {0};
    struct sockaddr_nl addr;
    struct nlmsghdr *nlh;

    while(1) {
        len = sizeof(addr);
        r_size = recvfrom(fd, (void *)buf, sizeof(buf), 0, (struct sockaddr *)&addr, &len);
        nlh = (struct nlmsghdr *)buf;
        for (; NLMSG_OK(nlh, (unsigned long)r_size); nlh = NLMSG_NEXT(nlh, r_size)) {
            switch (nlh->nlmsg_type) {
            case NLMSG_DONE:
            case NLMSG_ERROR:
                break;
            case RTM_NEWLINK:
            case RTM_DELLINK:
                nl_netifinfo_handle(nlh);
                break;
            case RTM_NEWADDR:
            case RTM_DELADDR:
                nl_netifaddr_handle(nlh);
                break;
            case RTM_NEWROUTE:
            case RTM_DELROUTE:
                nl_netroute_handle(nlh);
                break;
            default:
                break;
            }
        }
    }
}

void epoll_event_handle(void)
{
    int i = 0;
    int fd_cnt = 0;
    int sfd;
    struct epoll_event events[EPOLL_LISTEN_MAX_CNT];

    memset(events, 0, sizeof(events));
    while (1) {
        fd_cnt = epoll_wait(g_epollfd, events, EPOLL_LISTEN_MAX_CNT, EPOLL_LISTEN_TIMEOUT);
        for (i = 0; i < fd_cnt; i++) {
            sfd = events[i].data.fd;
            if (events[i].events & EPOLLIN) {
                if (sfd == g_nlfd) {
                    nl_netlink_hanlde(sfd);
                }
            }
        }
    }
}

int epoll_add_fd(int fd)
{
    struct epoll_event ev;
    ev.data.fd = fd;
    ev.events = EPOLLIN | EPOLLET;

    if (epoll_ctl(g_epollfd, EPOLL_CTL_ADD, fd, &ev) < 0) {
        perror("epoll add fd error");
        return -1;
    }

    printf("epoll add fd[%d] success\n", fd);
    return 0;
}

int init_epoll_fd()
{
    int epollfd = -1;
    epollfd = epoll_create(EPOLL_LISTEN_MAX_CNT);
    if (epollfd < 0) {
        perror("epoll create error");
        return -1;
    }

    g_epollfd = epollfd;

    printf("epoll create fd [%d] success\n", epollfd);
    return g_epollfd;
}

int init_nl_sockfd()
{
    int ret = 0;
    int nlfd = -1;
    struct sockaddr_nl sa;

    nlfd = socket(AF_NETLINK, SOCK_RAW, NETLINK_ROUTE);
    if (nlfd < 0) {
        perror("create netlink socket error");
        return -1;
    }

    memset(&sa, 0, sizeof(sa));
    sa.nl_family = AF_NETLINK;
    sa.nl_groups = RTMGRP_LINK | RTMGRP_IPV4_IFADDR | RTMGRP_IPV4_ROUTE | RTMGRP_IPV6_IFADDR | RTMGRP_IPV6_ROUTE;

    ret = bind(nlfd, (struct sockaddr *)&sa, sizeof(sa));
    if (ret < 0) {
        perror("bind error");
        close(nlfd);
        return -1;
    }

    if (epoll_add_fd(nlfd)) {
        close(nlfd);
        return -1;
    }

    g_nlfd = nlfd;
    printf("netlink create fd [%d] success\n", nlfd);
    return nlfd;
}

int main(int argc, char *argv[])
{
    if (init_epoll_fd() < 0) {
        return -1;
    }
    if (init_nl_sockfd() < 0) {
        return -1;
    }
    epoll_event_handle();
    return 0;
}
