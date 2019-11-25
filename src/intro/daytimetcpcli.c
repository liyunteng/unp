/*
 * daytimetcpcli.c - daytimetcpcli
 *
 * Date   : 2019/11/04
 */

#include "unp.h"

int
main(int argc, char *argv[])
{
    int                sockfd, n;
    char               recvline[MAXLINE + 1];
    struct sockaddr_in serveraddr;

    if (argc != 2) {
        err_quit("Usage: %s <IPaddress>", argv[0]);
    }

    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        err_sys("socket error");
    }

    bzero(&serveraddr, sizeof(serveraddr));
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_port   = htons(13);
    if (inet_pton(AF_INET, argv[1], &serveraddr.sin_addr) <= 0) {
        err_quit("inet_pton error for %s", argv[1]);
    }

    if (connect(sockfd, (struct sockaddr *)&serveraddr, sizeof(serveraddr)) < 0) {
        err_sys("connect error");
    }

    while ((n = read(sockfd, recvline, MAXLINE)) > 0) {
        recvline[n] = 0;
        if (fputs(recvline, stdout) == EOF) {
            err_sys("fputs error");
        }
    }

    if (n < 0) {
        err_sys("read error");
    }
    return 0;
}
