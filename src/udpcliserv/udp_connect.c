/*
 * udp_connect.c - udp_connect
 *
 * Date   : 2019/12/02
 */
#include "unp.h"

int
main(int argc, char *argv[])
{
    int sockfd, n;
    char sendline[MAXLINE], recvline[MAXLINE + 1];

    if (argc != 3) {
        err_quit("Usage: %s <host> <service>", argv[0]);
    }

    sockfd = udp_connect(argv[1], argv[2]);

    while (Fgets(sendline, MAXLINE, stdin) != NULL) {
        Write(sockfd, sendline, strlen(sendline));

        n           = Read(sockfd, recvline, MAXLINE);
        recvline[n] = 0;
        Fputs(recvline, stdout);
    }

    return 0;
}
