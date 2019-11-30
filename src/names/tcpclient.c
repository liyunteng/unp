/*
 * tcpclient.c - tcpclient
 *
 * Date   : 2019/11/30
 */
#include "unp.h"

int
main(int argc, char *argv[])
{
    int sockfd, n;
    socklen_t len;
    struct sockaddr_storage ss;

    if (argc != 3) {
        err_quit("Usage: %s <host> <service>", argv[0]);
    }

    sockfd = tcp_connect(argv[1], argv[2]);

    len = sizeof(ss);
    Getpeername(sockfd, (SA *)&ss, &len);
    printf("connected to %s\n", Sock_ntop((SA *)&ss, len));

    str_cli_select02(stdin, sockfd);
    return 0;
}
