/*
 * libudp.c - libudp
 *
 * Date   : 2019/11/25
 */
#include "unp.h"

void
dg_echo(int sockfd, SA *pcliaddr, socklen_t clilen)
{
    int n;
    socklen_t len;
    char msg[MAXLINE];

    for (;;) {
        len = clilen;
        n   = Recvfrom(sockfd, msg, MAXLINE, 0, pcliaddr, &len);

        Sendto(sockfd, msg, n, 0, pcliaddr, len);
    }
}

void
dg_cli(FILE *fp, int sockfd, const SA *pservaddr, socklen_t servlen)
{
    int n;
    char sendline[MAXLINE], recvline[MAXLINE + 1];

    while (Fgets(sendline, MAXLINE, fp) != NULL) {
        Sendto(sockfd, sendline, strlen(sendline), 0, pservaddr, servlen);

        n           = Recvfrom(sockfd, recvline, MAXLINE, 0, NULL, NULL);
        recvline[n] = 0;
        Fputs(recvline, stdout);
    }
}

void
dg_cliconnect(FILE *fp, int sockfd, const SA *servaddr, socklen_t servlen)
{
    int n;
    char sendline[MAXLINE], recvline[MAXLINE + 1];

    Connect(sockfd, (SA *)servaddr, servlen);

    while (Fgets(sendline, MAXLINE, fp) != NULL) {
        Write(sockfd, sendline, strlen(sendline));

        n           = Read(sockfd, recvline, MAXLINE);
        recvline[n] = 0;
        Fputs(recvline, stdout);
    }
}

void
dg_cli01(FILE *fp, int sockfd, const SA *servaddr, socklen_t servlen)
{
    int n;
    char sendline[MAXLINE], recvline[MAXLINE + 1];
    socklen_t len;
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

#define NDG 200000
#define DGLEN 1400

void
dg_cliloop(FILE *fp, int sockfd, const SA *servaddr, socklen_t servlen)
{
    int i;
    char sendline[DGLEN];

    for (i = 0; i < NDG; i++) {
        Sendto(sockfd, sendline, DGLEN, 0, servaddr, servlen);
    }
}

static int count;
static void
recvfrom_int(int signu)
{
    printf("\nreceived %d datagrams\n", count);
    exit(0);
}
void
dg_echoloop(int sockfd, SA *cliaddr, socklen_t clilen)
{
    socklen_t len;
    char msg[MAXLINE];

    Signal(SIGINT, recvfrom_int);

    for (;;) {
        len = clilen;
        Recvfrom(sockfd, msg, MAXLINE, 0, cliaddr, &len);
        count++;
    }
}

int
udp_client(const char *host, const char *serv, SA **saptr, socklen_t *lenp)
{
    int sockfd, n;
    struct addrinfo hints, *res, *ressave;

    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family   = AF_UNSPEC;
    hints.ai_socktype = SOCK_DGRAM;
    if (host != NULL && inet_aton(host, NULL) == 1) {
        hints.ai_flags |= AI_NUMERICHOST;
    }
    if (serv != NULL && atoi(serv) > 0) {
        hints.ai_flags |= AI_NUMERICSERV;
    }

    if ((n = getaddrinfo(host, serv, &hints, &res)) != 0) {
        err_quit("udp_client error for %s, %s: %s", host, serv,
                 gai_strerror(n));
    }
    ressave = res;

    do {
        sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
        if (sockfd >= 0)
            break;
    } while ((res = res->ai_next) != NULL);

    if (res == NULL)
        err_sys("udp_client error for %s, %s", host, serv);

    *saptr = Malloc(res->ai_addrlen);
    memcpy(*saptr, res->ai_addr, res->ai_addrlen);
    *lenp = res->ai_addrlen;

    freeaddrinfo(ressave);
    return (sockfd);
}

int
udp_connect(const char *host, const char *serv)
{
    int sockfd, n;
    struct addrinfo hints, *res, *ressave;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family   = AF_UNSPEC;
    hints.ai_socktype = SOCK_DGRAM;

    if (host != NULL && inet_aton(host, NULL) == 1) {
        hints.ai_flags |= AI_NUMERICHOST;
    }
    if (serv != NULL && atoi(serv) > 0) {
        hints.ai_flags |= AI_NUMERICSERV;
    }

    if ((n = getaddrinfo(host, serv, &hints, &res)) != 0) {
        err_quit("udp_connect error for %s, %s: %s", host, serv,
                 gai_strerror(n));
    }
    ressave = res;

    do {
        sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
        if (sockfd < 0)
            continue;

        if (connect(sockfd, res->ai_addr, res->ai_addrlen) == 0)
            break;
    } while ((res = res->ai_next) != NULL);

    if (res == NULL)
        err_sys("udp_connect error for %s, %s", host, serv);

    freeaddrinfo(ressave);

    return (sockfd);
}

int
udp_server(const char *host, const char *serv, socklen_t *lenp)
{
    int sockfd, n;
    struct addrinfo hints, *res, *ressave;

    memset(&hints, 0, sizeof(hints));
    hints.ai_flags    = AI_PASSIVE;
    hints.ai_family   = AF_UNSPEC;
    hints.ai_socktype = SOCK_DGRAM;

    if (host != NULL && inet_aton(host, NULL) == 1) {
        hints.ai_flags |= AI_NUMERICHOST;
    }
    if (serv != NULL && atoi(serv) > 0) {
        hints.ai_flags |= AI_NUMERICSERV;
    }

    if ((n = getaddrinfo(host, serv, &hints, &res)) != 0) {
        err_quit("udp_server error for %s, %s: %s", host, serv,
                 gai_strerror(n));
    }
    ressave = res;

    do {
        sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
        if (sockfd < 0)
            continue;

        if (bind(sockfd, res->ai_addr, res->ai_addrlen) == 0)
            break;

        Close(sockfd);
    } while ((res = res->ai_next) != NULL);

    if (res == NULL) {
        err_sys("udp_server error for %s, %s", host, serv);
    }

    if (lenp)
        *lenp = res->ai_addrlen;

    return (sockfd);
}
