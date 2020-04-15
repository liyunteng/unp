/*
 * server.c - server
 *
 * Date   : 2020/04/16
 */
#include <sys/socket.h>
#include <errno.h>
#include <assert.h>
#include <string.h>
#include <arpa/inet.h>
#include "libp2p.h"

static int
udp_server_recv_data(int sockfd, struct epoll_event ev, char *buf, size_t len)
{
    int n, sn;
    struct sockaddr cliaddr;
    socklen_t clilen = sizeof(cliaddr);
    char send_buf[BUFFSIZE];
    if(ev.events & EPOLLIN) {
        n = recvfrom(sockfd, buf, len, 0, (struct sockaddr *)&cliaddr, &clilen);
        if (n > 0) {
            Debug("recv: %s", buf);
            switch (cliaddr.sa_family) {
            case AF_INET: {
                struct sockaddr_in *sin = (struct sockaddr_in *)&cliaddr;
                inet_ntop(cliaddr.sa_family, &sin->sin_addr, send_buf, sizeof(send_buf));
                break;
            }
            case AF_INET6: {
                struct sockaddr_in6 *sin6 = (struct sockaddr_in6 *)&cliaddr;
                inet_ntop(cliaddr.sa_family, &sin6->sin6_addr, send_buf, sizeof(send_buf));
                break;
            }
            default:
                break;
            }
            sn = sendto(sockfd, send_buf, strlen(send_buf), 0, (struct sockaddr *)&cliaddr, clilen);
            Debug("send: %s", send_buf);
            return n;
        } else if (n == 0) {
            Info("disconnected");
        } else {
            if (errno == EINTR || errno == EAGAIN || errno == EWOULDBLOCK) {
                Info("EINTR|EAGAIN");
            } else if (errno == ECONNABORTED || errno == ECONNREFUSED || errno == ECONNRESET) {
                Info("ECONNABORTED|ECONNrEFUSED|ECONNRESET");
            } else {
                Error("%s", strerror(errno));
            }
        }
    } else if (ev.events & EPOLLERR) {
        Warn("EPOLLERR");
    } else if (ev.events & EPOLLRDHUP) {
        Warn("EPOLLRDHUP");
    } else if (ev.events & EPOLLHUP) {
        Warn("EPOLLHUP");
    }
    return -1;
}

static int
server(const char *host, const char *service)
{
    int sockfd, rc;
    socklen_t salen;
    int epfd, nfds;
    struct epoll_event ev, events[MAXEVENTS];
    int timeout = 3 * 1000;
    char recv_buf[BUFFSIZE];

    sockfd = udp_server_sockfd(host, service, &salen);
    if (sockfd < 0) {
        return -1;
    }

    rc = udp_set_nonblock(sockfd);
    assert(rc == 0);

    epfd = epoll_create(1);
    assert(epfd > 0);

    ev.events = EPOLLIN;
    ev.data.fd = sockfd;
    if (epoll_ctl(epfd, EPOLL_CTL_ADD, sockfd, &ev) == -1) {
        Error("epoll_ctl EPOLL_CTL_ADD failed: %s", strerror(errno));
        return -1;
    }

    for (;;) {
        nfds = epoll_wait(epfd, events, MAXEVENTS, timeout);
        if (nfds == -1) {
            if (errno == EINTR) {
                continue;
            } else {
                Error("epoll_wait failed: %s", strerror(errno));
                return -1;
            }
        }

        if (nfds == 0) {
            /* udp_send_data(sockfd, send_buf, strlen(send_buf)); */
            continue;
        }

        for (int i = 0; i < nfds; i++) {
            if (events[i].data.fd == sockfd) {
                rc = udp_server_recv_data(sockfd, events[i], recv_buf, sizeof(recv_buf));
                if (rc < 0) {
                    return -1;
                }
            }
        }
    }
}

int main(int argc, char *argv[])
{
    if (argc != 3) {
        Error("Usage: %s host service", argv[0]);
        return -1;
    }
    server(argv[1], argv[2]);
    return 0;
    return 0;
}

/* Local Variables: */
/* compile-command: "clang -Wall -o server server.c -g libp2p.c" */
/* End: */
