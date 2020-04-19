/*
 * client.c - client
 *
 * Date   : 2020/04/15
 */

#include <sys/socket.h>
#include <errno.h>
#include <assert.h>
#include <string.h>
#include <sys/epoll.h>

#include "log.h"
#include "udp.h"
#include "libp2p.h"

static int
udp_client_send_data(int sockfd, uint8_t *buf, size_t len)
{
    int n;
    n = send(sockfd, buf, len, 0);
    assert(n == (int)len);
    return n;
}

static void
dump_response(uint8_t *buf, size_t len)
{
    for (size_t i = 0; i < len; i++) {
        printf("0x%02X ", buf[i]);
    }
    printf("\n");
}

static int
udp_client_recv_data(int sockfd, struct epoll_event ev, uint8_t *buf, size_t len)
{
    int n;
    if(ev.events & EPOLLIN) {
        n = recv(sockfd, buf, len, 0);
        if (n > 0) {
            /* dump_response(buf, n); */
            /* stun_parse_response(buf,n); */
            stun_msg_dump(buf);
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
client(const char *host, const char *service)
{
    int sockfd, rc;
    socklen_t salen;
    struct sockaddr_storage *sa;
    int epfd, nfds;
    struct epoll_event ev, events[MAXEVENTS];
    int timeout = 3 * 1000;
    uint8_t send_buf[BUFFSIZE];
    int send_len = 0;
    uint8_t recv_buf[BUFFSIZE];
    size_t buflen;

    sockfd = udp_client_sockfd(host, service, (struct sockaddr **)&sa, &salen);
    if (sockfd < 0) {
        return -1;
    }

    if (connect(sockfd, (struct sockaddr *)sa, salen) < 0) {
        Error("connect failed: %s", strerror(errno));
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

    buflen = sizeof(send_buf);
    stun_trans_id id;
    stun_make_trans_id(id);
    send_len = stun_get_binding_request(send_buf, buflen, id);
    if (send_len < 0) {
        Error("get binding request failed\n");
        return -1;
    }
    dump_response(send_buf,send_len);
    udp_client_send_data(sockfd, send_buf, send_len);
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
            udp_client_send_data(sockfd, send_buf, send_len);
        }

        for (int i = 0; i < nfds; i++) {
            if (events[i].data.fd == sockfd) {
                rc = udp_client_recv_data(sockfd, events[i], recv_buf, sizeof(recv_buf));
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
    client(argv[1], argv[2]);
    return 0;
}

/* Local Variables: */
/* compile-command: "clang -Wall -o client client.c -g libp2p.c udp.c" */
/* End: */
