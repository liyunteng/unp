/*
 * daytimetcpsrv.c - daytimetcpsrv
 *
 * Date   : 2019/11/05
 */
#include "unp.h"

int
main(void)
{
    int listenfd, connfd;
    struct sockaddr_in servaddr;
    char buf[MAXLINE];
    time_t ticks;

    listenfd = Socket(AF_INET, SOCK_STREAM, 0);
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family      = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port        = htons(13);

    Bind(listenfd, (SA *)&servaddr, sizeof(servaddr));
    Listen(listenfd, LISTENQ);

    for (;;) {
        connfd = Accept(listenfd, NULL, NULL);

        ticks = time(NULL);
        snprintf(buf, sizeof(buf), "%.24s\r\n", ctime(&ticks));
        Write(connfd, buf, strlen(buf));
        Close(connfd);
    }
    return 0;
}
