/*
 * libudp.c - libudp
 *
 * Date   : 2019/11/25
 */
#include "unp.h"

void
dg_echo(int sockfd, SA *pcliaddr, socklen_t clilen)
{
    int       n;
    socklen_t len;
    char      msg[MAXLINE];

    for (;;) {
        len = clilen;
        n   = Recvfrom(sockfd, msg, MAXLINE, 0, pcliaddr, &len);

        Sendto(sockfd, msg, n, 0, pcliaddr, len);
    }
}

void
dg_cli(FILE *fp, int sockfd, const SA *pservaddr, socklen_t servlen)
{
    int  n;
    char sendline[MAXLINE], recvline[MAXLINE + 1];

    while (Fgets(sendline, MAXLINE, fp) != NULL) {
        Sendto(sockfd, sendline, strlen(sendline), 0, pservaddr, servlen);

        n           = Recvfrom(sockfd, recvline, MAXLINE, 0, NULL, NULL);
        recvline[n] = 0;
        Fputs(recvline, stdout);
    }
}

void
dg_cli01(FILE *fp, int sockfd, const SA *servaddr, socklen_t servlen)
{
    int              n;
    char             sendline[MAXLINE], recvline[MAXLINE + 1];
    socklen_t        len;
    struct sockaddr *preply_addr;

    preply_addr = Malloc(servlen);

    while (Fgets(sendline, MAXLINE, fp) != NULL) {
        Sendto(sockfd, sendline, strlen(sendline), 0, servaddr, servlen);

        len = servlen;
        n   = Recvfrom(sockfd, recvline, MAXLINE, 0, preply_addr, &len);
        if (len != servlen || memcmp(servaddr, preply_addr, len) != 0) {
            err_msg("reply from %s (ignored)\n", Sock_ntop(preply_addr, len));
            continue;
        }

        recvline[n] = 0;
        Fputs(recvline, stdout);
    }
}
