/*
 * udp_server.c - udp_server
 *
 * Date   : 2019/12/02
 */
#include "unp.h"

int
main(int argc, char *argv[])
{
    int sockfd;
    struct sockaddr_storage cliaddr;

    if (argc == 2) {
        sockfd = udp_server(NULL, argv[1], NULL);
    } else if (argc == 3) {
        sockfd = udp_server(argv[1], argv[2], NULL);
    } else {
        err_quit("Usage: %s [<host>] <service>", argv[0]);
    }

    dg_echo(sockfd, (SA *)&cliaddr, sizeof(cliaddr));
    return 0;
}
