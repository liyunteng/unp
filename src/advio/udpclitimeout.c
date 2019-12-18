/*
 * udpcli_timeo.c - udpcli_timeo
 *
 * Date   : 2019/12/04
 */
#include "unp.h"

int
main(int argc, char *argv[])
{
    int sockfd;
    socklen_t salen;
    struct sockaddr *sa;

    if (argc != 3) {
        err_quit("Usage: %s <host> <service>", argv[0]);
    }

    sockfd = udp_client(argv[1], argv[2], (SA **)&sa, &salen);
    printf("sending to %s\n", Sock_ntop(sa, salen));

    dg_cli_timeo(stdin, sockfd, sa, salen);

    return 0;
}
