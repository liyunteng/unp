/*
 * connect_timeo.c - connect_timeo
 *
 * Date   : 2019/12/04
 */
#include "unp.h"

int
main(int argc, char *argv[])
{
    int sockfd;
    int timeout = 3;
    struct sockaddr_in addr;

    if (argc < 3 || argc > 4) {
        err_quit("Usage: %s <IPaddress> <Port> [<Timeout>]", argv[0]);
    }
    if (argc == 4) {
        timeout = atoi(argv[3]);
    }

    sockfd = Socket(AF_INET, SOCK_STREAM, 0);
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port   = htons(atoi(argv[2]));
    Inet_pton(AF_INET, argv[1], &(addr.sin_addr));

    int n = connect_timeo(sockfd, (SA *)&addr, sizeof(addr), timeout);
    if (n < 0) {
        err_sys("connect_timeo");
    }

    return 0;
}
