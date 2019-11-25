/*
 * daytimetcpsrv1.c - daytimetcpsrv1
 *
 * Date   : 2019/11/05
 */
#include "unp.h"

int
main(void)
{
    int                listenfd, connfd;
    socklen_t          len;
    struct sockaddr_in servaddr, cliaddr;
    char               buf[MAXLINE];
    time_t             ticks;

    listenfd = Socket(AF_INET, SOCK_STREAM, 0);

    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family      = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port        = htons(13);

    Bind(listenfd, (SA *)&servaddr, sizeof(servaddr));
    Listen(listenfd, LISTENQ);

    for (;;) {
        len    = sizeof(cliaddr);
        connfd = Accept(listenfd, (SA *)&cliaddr, &len);
        printf("connection from %s, port %d\n",
               inet_ntop(AF_INET, &cliaddr.sin_addr, buf, sizeof(buf)), ntohs(cliaddr.sin_port));
        ticks = time(NULL);
        snprintf(buf, sizeof(buf), "%.24s\r\n", ctime(&ticks));
        Write(connfd, buf, strlen(buf));

        Close(connfd);
    }

    return 0;
}
