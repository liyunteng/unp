/*
 * epoll_server.c - epoll server
 *
 * Date   : 2019/11/10
 */
#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <inttypes.h>
#include <netinet/in.h>
#include <pthread.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/epoll.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

#define BUFFSIZE 4096
#define MAXEVENTS 10

static uint32_t g_id = 0;
typedef struct {
    uint32_t id;
    int      connfd;
    char     addr[32];
    char     recv_buf[BUFFSIZE];
    char     send_buf[BUFFSIZE];
    uint32_t send_length;
    uint32_t recv_length;
    uint32_t sent_length;
} context_t;

int
setnonblocking(int fd)
{
    int val;
    if ((val = fcntl(fd, F_GETFL, 0)) < 0) {
        perror("fcntl getfl");
        return -1;
    }
    val |= O_NONBLOCK;

    if ((val = fcntl(fd, F_SETFL, val)) < 0) {
        perror("fcntl setfl");
        return -1;
    }
    return 0;
}

int
get_response(context_t *ctx)
{
    if (ctx == NULL) {
        return -1;
    }
    ctx->send_length =
        snprintf(ctx->send_buf, sizeof(ctx->send_buf),
                 "HTTP/1.1 200 OK\r\nContent-Length: 26\r\n\r\nabcdefghijklmnopqrstuvwxyz");
    /* memset(ctx->send_buf, 'c', sizeof(ctx->send_buf)); */
    /* ctx->send_length = sizeof(ctx->send_buf); */
    ctx->sent_length = 0;

    return 0;
}

int
do_accept(int epollfd, struct epoll_event ev)
{
    int                connfd;
    int                listenfd = ev.data.fd;
    struct sockaddr_in cliaddr;
    socklen_t          clilen = sizeof(cliaddr);
    if (ev.events & EPOLLIN) {
        while (1) {
            connfd = accept(listenfd, (struct sockaddr *)&cliaddr, &clilen);
            if (connfd > 0) {
                /* TODO: release ctx if failed ?? */
                context_t *ctx = (context_t *)malloc(sizeof(context_t));
                if (ctx == NULL) {
                    goto quit;
                }
                memset(ctx, 0, sizeof(context_t));
                ctx->id     = g_id++;
                ctx->connfd = connfd;
                char ip[INET_ADDRSTRLEN];
                snprintf(ctx->addr, sizeof(ctx->addr) - 1, "[%d] %s:%d", ctx->id,
                         inet_ntop(AF_INET, &cliaddr.sin_addr, ip, clilen),
                         ntohs(cliaddr.sin_port));
                get_response(ctx);

                printf("%s connected\n", ctx->addr);
                setnonblocking(connfd);
                ev.data.ptr = ctx;
                ev.events   = EPOLLIN | EPOLLET;
                if (epoll_ctl(epollfd, EPOLL_CTL_ADD, connfd, &ev) == -1) {
                    perror("epoll_ctl ADD");
                    goto quit;
                }
            } else if (connfd == -1) {
                if (errno == EINTR) {
                    continue;
                } else if (errno == EAGAIN || errno == EWOULDBLOCK) {
                    break;
                } else if (errno == ECONNABORTED || errno == EPROTO) {
                    continue;
                } else {
                    perror("accept");
                    goto quit;
                }
            }
        }
    }
    return 0;

quit:
    return -1;
}

int
do_server(int epollfd, struct epoll_event ev)
{
    int        connfd = -1;
    int        n;
    context_t *ctx = ev.data.ptr;
    if (ctx == NULL) {
        goto quit;
    }

    connfd = ctx->connfd;

    if (ev.events & EPOLLIN) {
        /* for EPOLLET */
        while (ctx->recv_length < sizeof(ctx->recv_buf) - 1) {
            n = recv(connfd, ctx->recv_buf + ctx->recv_length,
                     sizeof(ctx->recv_buf) - ctx->recv_length - 1, 0);
            if (n > 0) {
                ctx->recv_length += n;
            } else if (n == 0) {
                /* peer closed */
                printf("%s disconnect\n", ctx->addr);
                shutdown(connfd, SHUT_WR);
                goto disconnect;
            } else if (n == -1) {
                if (errno == EINTR) {
                    continue;
                } else if (errno == EAGAIN || errno == EWOULDBLOCK) {
                    ctx->recv_buf[ctx->recv_length] = 0;
                    printf("%s", ctx->recv_buf);
                    fflush(stdout);
                    ev.data.ptr = ctx;
                    ev.events   = EPOLLIN | EPOLLOUT | EPOLLET;
                    if (epoll_ctl(epollfd, EPOLL_CTL_MOD, connfd, &ev) == -1) {
                        perror("epoll_ctl MOD");
                        goto quit;
                    }
                    break;
                } else if (errno == ECONNRESET) {
                    printf("%s reset by peer\n", ctx->addr);
                    close(connfd);
                    goto disconnect;
                } else {
                    perror("recv");
                    goto quit;
                }
            }
            /* recv_buf full, override */
            if (ctx->recv_length >= sizeof(ctx->recv_buf) - 1) {
                ctx->recv_buf[sizeof(ctx->recv_buf) - 1] = 0;
                printf("%s", ctx->recv_buf);
                /* printf("## buffer is full ##\n"); */
                ctx->recv_length = 0;
                ctx->recv_buf[0] = 0;
            }
        }
    } else if (ev.events & EPOLLOUT) {
        while (ctx->sent_length < ctx->send_length) {
            n = send(connfd, ctx->send_buf + ctx->sent_length, ctx->send_length - ctx->sent_length,
                     0);
            if (n > 0) {
                ctx->sent_length += n;
                printf("send to %s %d/%d [%d]\n", ctx->addr, ctx->sent_length, ctx->send_length, n);
            } else if (n == 0) {

            } else if (n == -1) {
                if (errno == EINTR) {
                    continue;
                } else if (errno == EAGAIN || errno == EWOULDBLOCK) {
                    break;
                } else if (errno == ECONNRESET) {
                    printf("%s reset by peer\n", ctx->addr);
                    close(connfd);
                    goto disconnect;
                } else {
                    perror("send");
                    goto quit;
                }
            }
        }
        printf("%s closed\n", ctx->addr);
        shutdown(connfd, SHUT_RDWR);
        goto disconnect;
    } else if (ev.events & EPOLLERR) {
        perror("EPOLLERR");
        goto quit;
    } else if (ev.events & EPOLLRDHUP) {
        perror("EPOLLRDHUP");
        goto quit;
    } else if (ev.events & EPOLLHUP) {
        perror("EPOLLHUP");
        goto quit;
    }
    return 0;

disconnect:
    free(ctx);
    ctx         = NULL;
    ev.data.ptr = NULL;
    epoll_ctl(epollfd, EPOLL_CTL_DEL, connfd, &ev);
    return 0;

quit:
    free(ctx);
    ctx         = NULL;
    ev.data.ptr = NULL;
    if (epoll_ctl(epollfd, EPOLL_CTL_DEL, connfd, &ev) == -1) {
        perror("epoll_ctl DEL");
    }
    return -1;
}

int
epoll_server(int port)
{
    int                epollfd, nfds;
    int                listenfd;
    struct sockaddr_in servaddr;
    struct epoll_event ev, events[MAXEVENTS];

    if ((listenfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket");
        return -1;
    }

    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family      = AF_INET;
    servaddr.sin_port        = htons(port);
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    setnonblocking(listenfd);
    int val = 1;
    if (setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &val, sizeof(val)) < 0) {
        perror("setsockopt");
        return -1;
    }

    if (setsockopt(listenfd, SOL_SOCKET, SO_REUSEPORT, &val, sizeof(val)) < 0) {
        perror("setsockopt");
        return -1;
    }
    if (bind(listenfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0) {
        perror("bind");
        return -1;
    }

    if (listen(listenfd, 10) < 0) {
        perror("listen");
        return -1;
    }

    epollfd = epoll_create(1);
    if (epollfd == -1) {
        perror("epoll_create1");
        return -1;
    }
    ev.events  = EPOLLIN | EPOLLET;
    ev.data.fd = listenfd;
    if (epoll_ctl(epollfd, EPOLL_CTL_ADD, listenfd, &ev) == -1) {
        perror("epoll_ctl");
        return -1;
    }
    for (;;) {
        nfds = epoll_wait(epollfd, events, MAXEVENTS, -1);
        if (nfds == -1) {
            if (errno == EINTR) {
                continue;
            } else {
                perror("epoll_wait");
                return -1;
            }
        }

        for (int i = 0; i < nfds; ++i) {
            if (events[i].data.fd == listenfd) {
                if (do_accept(epollfd, events[i]) < 0) {
                    return -1;
                }
            } else {
                if (do_server(epollfd, events[i]) < 0) {
                    return -1;
                }
            }
        }
    }
}

int
main(int argc, char *argv[])
{
    if (argc != 2) {
        printf("Usage: %s <Port>\n", argv[0]);
        return -1;
    }
    epoll_server(atoi(argv[1]));
    return 0;
}
