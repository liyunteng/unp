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
#include <stdlib.h>
#include <sys/epoll.h>

#include "log.h"
#include "udp.h"
#include "libp2p.h"

static int
udp_server_recv_data(int sockfd, struct epoll_event ev, uint8_t *buf, size_t len)
{
    int n, sn;
    struct sockaddr_storage cliaddr;
    socklen_t clilen = sizeof(cliaddr);
    uint8_t send_buf[BUFFSIZE];
    ssize_t send_len;
    char cli_buf[64];
    char port_buf[8];
    char id[32];
    if(ev.events & EPOLLIN) {
        n = recvfrom(sockfd, buf, len, 0, (struct sockaddr *)&cliaddr, &clilen);
        if (n > 0) {
            switch (cliaddr.ss_family) {
            case AF_INET: {
                struct sockaddr_in *sin = (struct sockaddr_in *)&cliaddr;
                inet_ntop(AF_INET, &sin->sin_addr, (char *)cli_buf, sizeof(cli_buf));
                if (ntohs(sin->sin_port) != 0) {
                    snprintf(port_buf, sizeof(port_buf), ":%d", ntohs(sin->sin_port));
                    strcat(cli_buf, port_buf);
                }
                break;
            }
            case AF_INET6: {
                struct sockaddr_in6 *sin6 = (struct sockaddr_in6 *)&cliaddr;
                if (inet_ntop(AF_INET6, &sin6->sin6_addr, (char *)cli_buf, sizeof(cli_buf)) == NULL) {
                    Error("ntop failed");
                }
                if (ntohs(sin6->sin6_port) != 0) {
                    snprintf(port_buf, sizeof(port_buf), ":%d", ntohs(sin6->sin6_port));
                    strcat(cli_buf, port_buf);
                }
                break;
            }
            default:
                break;
            }
            stun_header_t *binding = (stun_header_t *)buf;
            binding->msg_type = ntohs(binding->msg_type);
            binding->magic = ntohl(binding->magic);
            stun_trans_id_to_string(binding->id, id, sizeof(id));
            Debug("%s msg_type: %s(0x%04X) magic: 0x%X id: %s",
                  cli_buf,
                  stun_msg_type_to_string(binding->msg_type),
                  binding->msg_type,
                  binding->magic,
                  id);


            send_len = stun_get_binding_response(send_buf,sizeof(send_buf), binding->id, (struct sockaddr *)&cliaddr, clilen);
            if (send_len < 0) {
                Error("get binding response failed: %ld", send_len);
            }
            sn = sendto(sockfd, send_buf, send_len, 0, (struct sockaddr *)&cliaddr, clilen);
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
    uint8_t recv_buf[BUFFSIZE];

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
}

/* Local Variables: */
/* compile-command: "clang -Wall -o server server.c -g libp2p.c udp.c" */
/* End: */
