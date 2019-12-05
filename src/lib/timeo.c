/*
 * timeo.c - timeo
 *
 * Date   : 2019/12/04
 */
#include "unp.h"

static void
connect_alarm(int signo)
{
    return; /* just interrupt the connect */
}
int
connect_timeo(int sockfd, const SA *saptr, socklen_t salen, int nsec)
{
    struct sigaction act, oact;
    int n;

    memset(&act, 0, sizeof(act));
    sigemptyset(&act.sa_mask);
    /* act.sa_flags = SA_RESTART; */
    act.sa_flags   = 0; /* don't restart */
    act.sa_handler = connect_alarm;

    if (sigaction(SIGALRM, &act, &oact) != 0) {
        err_sys("sigaction failed");
    }

    if (alarm(nsec) != 0) {
        err_msg("connect_timeo: alarm was already set");
    }

    if ((n = connect(sockfd, saptr, salen)) < 0) {
        close(sockfd);

        /* interrupt by SIGALRM */
        if (errno == EINTR)
            errno = ETIMEDOUT;
    }
    alarm(0);
    sigaction(SIGALRM, &oact, NULL);
    return (n);
}

void
dg_cli_signal_timeo(FILE *fp, int sockfd, const SA *pservaddr,
                    socklen_t servlen)
{
    int n;
    char sendline[MAXLINE], recvline[MAXLINE + 1];
    struct sigaction act, oact;

    memset(&act, 0, sizeof(act));
    sigemptyset(&act.sa_mask);
    /* act.sa_flags = SA_RESTART; */
    act.sa_flags   = 0; /* don't restart */
    act.sa_handler = connect_alarm;

    Sigaction(SIGALRM, &act, &oact);

    while (Fgets(sendline, MAXLINE, fp) != 0) {
        Sendto(sockfd, sendline, strlen(sendline), 0, pservaddr, servlen);

        alarm(5);
        if ((n = recvfrom(sockfd, recvline, MAXLINE, 0, NULL, NULL)) < 0) {
            if (errno == EINTR) {
                fprintf(stderr, "socket timeout\n");
            } else {
                err_sys("recvfrom error");
            }
        } else {
            alarm(0);
            recvline[n] = 0;
            Fputs(recvline, stdout);
        }
    }
}

int
readable_timeo(int fd, int sec)
{
    fd_set rset;
    struct timeval tv;

    FD_ZERO(&rset);
    FD_SET(fd, &rset);

    tv.tv_sec  = sec;
    tv.tv_usec = 0;

    return (select(fd + 1, &rset, NULL, NULL, &tv));
}

void
dg_cli_timeo(FILE *fp, int sockfd, const SA *pservaddr, socklen_t servlen)
{
    int n;
    char sendline[MAXLINE], recvline[MAXLINE + 1];

    while (Fgets(sendline, MAXLINE, fp) != 0) {
        Sendto(sockfd, sendline, strlen(sendline), 0, pservaddr, servlen);

        if (readable_timeo(sockfd, 5) == 0) {
            fprintf(stderr, "socket timeout\n");
        } else {
            n           = Recvfrom(sockfd, recvline, MAXLINE, 0, NULL, NULL);
            recvline[n] = 0;
            Fputs(recvline, stdout);
        }
    }
}

void
dg_cli_sockopt(FILE *fp, int sockfd, const SA *pservaddr, socklen_t serlen)
{
    int n;
    char sendline[MAXLINE], recvline[MAXLINE + 1];
    struct timeval tv;

    tv.tv_sec  = 5;
    tv.tv_usec = 0;

    Setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));

    while (Fgets(sendline, MAXLINE, fp) != 0) {
        Sendto(sockfd, sendline, strlen(sendline), 0, pservaddr, servlen);

        n = recvfrom(sockfd, recvline, MAXLINE, 0, NULL, NULL);
        if (n < 0) {
            if (errno == EWOULDBLOCK) {
                fprintf(stderr, "socket timeout\n");
                continue;
            } else
                err_sys("recvfrom error");
        }

        recvline[n] = 0;
        Fputs(recvline, stdout);
    }
}
