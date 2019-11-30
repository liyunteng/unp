/*
 * daytimetcpsrv01.c - daytimetcpsrv01
 *
 * Date   : 2019/11/30
 */
#include "unp.h"
#include <time.h>

int
main(int argc, char *argv[])
{
    int listenfd, connfd;
    socklen_t len;
    char buf[MAXLINE];
    time_t ticks;
    struct sockaddr_storage cliaddr;

    if (argc != 3) {
        err_quit("Usage: %s <host> <service>", argv[0]);
    }

    listenfd = tcp_listen(argv[1], argv[2], NULL);

    for (;;) {
        len    = sizeof(cliaddr);
        connfd = Accept(listenfd, (SA *)&cliaddr, &len);
        printf("connection from %s\n", Sock_ntop((SA *)&cliaddr, len));

        ticks = time(NULL);
        snprintf(buf, sizeof(buf), "%.24s\r\n", ctime(&ticks));
        Write(connfd, buf, strlen(buf));

        close(connfd);
    }
    return 0;
}
