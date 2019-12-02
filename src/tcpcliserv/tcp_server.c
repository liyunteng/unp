/*
 * tcp_server.c - tcp_server
 *
 * Date   : 2019/11/30
 */
#include "unp.h"

#ifndef OPEN_MAX
#    define OPEN_MAX 32
#endif

int
main(int argc, char *argv[])
{
    int listenfd = -1, connfd, sockfd;
    int nready, i, maxi, n;
    char buf[MAXLINE];
    socklen_t clilen, addrlen;
    struct sockaddr_storage cliaddr;
    struct pollfd client[OPEN_MAX];

    if (argc == 2) {
        listenfd = tcp_listen(NULL, argv[1], &addrlen);
    } else if (argc == 3) {
        listenfd = tcp_listen(argv[1], argv[2], &addrlen);
    } else {
        err_quit("Usage: %s [<host>] <service>", argv[0]);
    }

    client[0].fd     = listenfd;
    client[0].events = POLLRDNORM;
    for (i = 1; i < OPEN_MAX; i++) {
        client[i].fd = -1;
    }
    maxi = 0;

    for (;;) {
        nready = Poll(client, maxi + 1, -1);

        if (client[0].revents & POLLRDNORM) {
            clilen = sizeof(cliaddr);
            connfd = Accept(listenfd, (SA *)&cliaddr, &clilen);
            /* Getpeername(connfd, (SA *)&cliaddr, &clilen); */
            err_msg("%s connected", Sock_ntop((SA *)&cliaddr, clilen));

            for (i = 1; i < OPEN_MAX; i++) {
                if (client[i].fd < 0) {
                    client[i].fd = connfd;
                    break;
                }
            }

            if (i == OPEN_MAX) {
                err_quit("too many clients");
            }

            client[i].events = POLLRDNORM;
            if (i > maxi)
                maxi = i;

            if (--nready <= 0)
                continue;
        }

        for (i = 0; i <= maxi; i++) {
            if ((sockfd = client[i].fd) < 0)
                continue;

            if (client[i].revents & (POLLRDNORM | POLLERR)) {
                if ((n = read(sockfd, buf, MAXLINE)) < 0) {
                    if (errno == ECONNRESET) {
                        Getpeername(sockfd, (SA *)&cliaddr, &clilen);
                        err_msg("%s reset by peer",
                                Sock_ntop((SA *)&cliaddr, clilen));
                        Close(sockfd);
                        client[i].fd = -1;
                    } else {
                        err_sys("read error");
                    }
                } else if (n == 0) {
                    Getpeername(sockfd, (SA *)&cliaddr, &clilen);
                    err_msg("%s disconnected",
                            Sock_ntop((SA *)&cliaddr, clilen));
                    Close(sockfd);
                    client[i].fd = -1;
                } else {
                    Writen(sockfd, buf, n);
                }

                if (--nready <= 0)
                    break;
            }
        }
    }

    return 0;
}
