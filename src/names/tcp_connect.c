/*
 * tcp_connect.c - tcp_connect
 *
 * Date   : 2019/11/30
 */
#include "unp.h"

int main(int argc, char *argv[])
{
    if (argc != 3) {
        err_quit("Usage: %s <host> <service>", argv[0]);
    }

    int sockfd = tcp_connect(argv[1], argv[2]);
    printf("sockfd: %d\n", sockfd);
    return 0;
}
