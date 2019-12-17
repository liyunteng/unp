/*
 * dgclibcast.c - dgclibcast
 *
 * Date   : 2019/12/16
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

    dg_cli_broadcast(stdin, sockfd, (SA *)&servaddr, sizeof(servaddr));

    return 0;
}
