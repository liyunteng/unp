/*
 * udpcli01.c - udpcli01
 *
 * Date   : 2019/11/25
 */
#include "unp.h"

int
main(int argc, char *argv[])
{
    int sockfd;
    struct sockaddr_in servaddr;

    if (argc != 2) {
        err_quit("Usage: %s <IPaddress>", argv[0]);
    }

    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port   = htons(SERV_PORT);
    Inet_pton(AF_INET, argv[1], &servaddr.sin_addr);

    sockfd = Socket(AF_INET, SOCK_DGRAM, 0);

    /* if not connect, recvfrom can't return error when server not start */
    /* Connect(sockfd, (SA *)&servaddr, sizeof(servaddr)); */

    dg_cliconnect(stdin, sockfd, (SA *)&servaddr, sizeof(servaddr));
    /* dg_cliloop(stdin, sockfd, (SA *)&servaddr, sizeof(servaddr)); */

    return 0;
}
