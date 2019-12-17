/*
 * broadcast.c - broadcast
 *
 * Date   : 2019/12/16
 */

#include "unp.h"
static void recvfrom_alarm(int);

void
dg_cli_broadcast(FILE *fp, int sockfd, const SA*pservaddr, socklen_t servlen)
{
    int n;
    const int on = 1;
    char sendline[MAXLINE], recvline[MAXLINE+1];
    socklen_t len;
    struct sockaddr *preply_addr;
    struct sigaction act, oact;

    preply_addr = Malloc(servlen);

    /* need server setsockopt too!! */
    Setsockopt(sockfd, SOL_SOCKET, SO_BROADCAST, &on, sizeof(on));

    act.sa_handler = recvfrom_alarm;
    act.sa_flags = SA_INTERRUPT;
    Sigaction(SIGALRM, &act, &oact);

    while(Fgets(sendline, MAXLINE, fp) != NULL) {
        Sendto(sockfd, sendline, strlen(sendline), 0, pservaddr, servlen);
        alarm(5);
        for (;;) {
            len = servlen;
            n = recvfrom(sockfd, recvline, MAXLINE, 0, preply_addr, &len);
            if (n < 0) {
                if (errno == EINTR) {
                    printf("break\n");
                    break;
                } else {
                    err_sys("recvfrom error");
                }
            } else {
                recvline[n] = 0;
                printf("from %s: %s\n", Sock_ntop(preply_addr, len), recvline);
            }
        }
    }
    free(preply_addr);
}

static void
recvfrom_alarm(int signo)
{
    return;
}
