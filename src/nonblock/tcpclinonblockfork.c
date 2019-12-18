/*
 * tcpcli_nonblock_fork.c - tcpcli_nonblock_fork
 *
 * Date   : 2019/12/07
 */
#include "unp.h"

int
main(int argc, char *argv[])
{
    int sockfd;
    struct sockaddr_in servaddr;
    int port = SERV_PORT;

    if (argc < 2 || argc > 3) {
        err_quit("Usage: %s <IPaddress> [<port>]", argv[0]);
    } else if (argc == 3) {
        port = atoi(argv[2]);
    }
    sockfd = Socket(AF_INET, SOCK_STREAM, 0);
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port   = htons(port);
    Inet_pton(AF_INET, argv[1], &servaddr.sin_addr);

    if (connect_nonblock(sockfd, (SA *)&servaddr, sizeof(servaddr), 5) < 0) {
        err_sys("connect nonblock error");
    }
    str_cli_nonblock_fork(stdin, sockfd);
    return 0;
}
