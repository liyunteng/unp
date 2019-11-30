/*
 * libtcp.c - libtcp
 *
 * Date   : 2019/11/25
 */
#include "unp.h"

void
str_echo(int sockfd)
{
    ssize_t n;
    char buf[MAXLINE];

again:
    while ((n = read(sockfd, buf, MAXLINE)) > 0) {
        writen(sockfd, buf, n);
    }

    if (n < 0 && errno == EINTR)
        goto again;
    else if (n < 0)
        err_sys("str_echo: read error");
}

void
str_echo08(int sockfd)
{
    long arg1, arg2;
    ssize_t n;
    char line[MAXLINE];

    for (;;) {
        if ((n = Readline(sockfd, line, MAXLINE)) == 0) {
            return;
        }
        if (sscanf(line, "%ld %ld", &arg1, &arg2) == 2) {
            snprintf(line, sizeof(line), "%ld\n", arg1 + arg2);
        } else {
            snprintf(line, sizeof(line), "input error\n");
        }
        n = strlen(line);
        Writen(sockfd, line, n);
    }
}

void
str_cli(FILE *fp, int sockfd)
{
    char sendline[MAXLINE], recvline[MAXLINE];

    while (Fgets(sendline, MAXLINE, fp) != NULL) {
        Write(sockfd, sendline, strlen(sendline));

        if (readline(sockfd, recvline, MAXLINE) == 0) {
            err_quit("str_cli: server terminated prematurely");
        }
        Fputs(recvline, stdout);
    }
}

struct args {
    long arg1;
    long arg2;
};
struct result {
    long sum;
};
void
bin_echo(int sockfd)
{
    ssize_t n;
    struct args args;
    struct result result;

    for (;;) {
        if ((n = Readn(sockfd, &args, sizeof(args))) == 0)
            return;

        result.sum = args.arg1 + args.arg2;
        Writen(sockfd, &result, sizeof(result));
    }
}
void
bin_cli(FILE *fp, int sockfd)
{
    char sendline[MAXLINE];
    struct args args;
    struct result result;

    while (Fgets(sendline, MAXLINE, fp) != NULL) {
        if (sscanf(sendline, "%ld %ld", &args.arg1, &args.arg2) != 2) {
            printf("invalid input: %s", sendline);
            continue;
        }

        Writen(sockfd, &args, sizeof(args));

        if (Readn(sockfd, &result, sizeof(result)) == 0) {
            err_quit("server terminated prematurely");
        }
        printf("%ld\n", result.sum);
    }
}

void
str_cli_select(FILE *fp, int sockfd)
{
    int maxfd;
    fd_set rset;
    char sendline[MAXLINE], recvline[MAXLINE];

    FD_ZERO(&rset);
    for (;;) {
        FD_SET(fileno(fp), &rset);
        FD_SET(sockfd, &rset);
        maxfd = max(fileno(fp), sockfd) + 1;
        Select(maxfd, &rset, NULL, NULL, NULL);

        if (FD_ISSET(sockfd, &rset)) {
            if (Readline(sockfd, recvline, MAXLINE) == 0) {
                err_quit("server terminated prematurely");
            }
            Fputs(recvline, stdout);
        }
        if (FD_ISSET(fileno(fp), &rset)) {
            if (Fgets(sendline, MAXLINE, fp) == NULL) {
                return;
            }
            Writen(sockfd, sendline, strlen(sendline));
        }
    }
}

void
str_cli_select02(FILE *fp, int sockfd)
{
    int maxfd, stdineof;
    fd_set rset;
    char buf[MAXLINE];
    int n;

    stdineof = 0;
    FD_ZERO(&rset);
    for (;;) {
        if (stdineof == 0)
            FD_SET(fileno(fp), &rset);

        FD_SET(sockfd, &rset);
        maxfd = max(fileno(fp), sockfd) + 1;
        Select(maxfd, &rset, NULL, NULL, NULL);

        if (FD_ISSET(sockfd, &rset)) {
            if ((n = Read(sockfd, buf, MAXLINE)) == 0) {
                if (stdineof == 1) {
                    return; /* normal termination */
                } else {
                    err_quit("server terminated prematurely");
                }
            }
            Writen(fileno(stdout), buf, n);
        }

        if (FD_ISSET(fileno(fp), &rset)) {
            if ((n = Read(fileno(fp), buf, MAXLINE)) == 0) {
                stdineof = 1;
                Shutdown(sockfd, SHUT_WR); /* send FIN */
                FD_CLR(fileno(fp), &rset);
                continue;
            }
            Writen(sockfd, buf, n);
        }
    }
}

int
tcp_connect(const char *host, const char *serv)
{
    int sockfd, n;
    struct addrinfo hints, *res, *ressave;

    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_UNSPEC;
    if (host != NULL && inet_aton(host, NULL) == 1) {
        hints.ai_flags |= AI_NUMERICHOST;
    }
    if (serv != NULL & atoi(serv) > 0) {
        hints.ai_flags |= AI_NUMERICSERV;
    }
    /* hints.ai_protocol = IPPROTO_TCP; */
    hints.ai_socktype = SOCK_STREAM;

    if ((n = getaddrinfo(host, serv, &hints, &res)) != 0) {
        err_quit("tcp_connect error for %s, %s: %s", host, serv,
                 gai_strerror(n));
    }
    ressave = res;

    do {
        sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
        if (sockfd < 0)
            continue;

        if (connect(sockfd, res->ai_addr, res->ai_addrlen) == 0)
            break;

        Close(sockfd);
    } while ((res = res->ai_next) != NULL);

    if (res == NULL)
        err_sys("tcp_connect error for %s, %s", host, serv);

    freeaddrinfo(ressave);

    return (sockfd);
}

int
tcp_listen(const char *host, const char *serv, socklen_t *addrlen)
{
    int listenfd, n;
    const int on = 1;
    struct addrinfo hints, *res, *ressave;

    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_flags    = AI_PASSIVE;
    if (host != NULL && inet_aton(host, NULL) == 1) {
        hints.ai_flags |= AI_NUMERICHOST;
    }
    if (serv != NULL && atoi(serv) > 0) {
        hints.ai_flags |= AI_NUMERICSERV;
    }
    hints.ai_family   = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    if ((n = getaddrinfo(host, serv, &hints, &res)) != 0) {
        err_quit("tcp_listen error for %s, %s: %s", host, serv,
                 gai_strerror(n));
    }
    ressave = res;

    do {
        listenfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
        if (listenfd < 0)
            continue;

        Setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));
        if (bind(listenfd, res->ai_addr, res->ai_addrlen) == 0)
            break;

        Close(listenfd);
    } while ((res = res->ai_next) != NULL);

    if (res == NULL)
        err_sys("tcp_listen error for %s, %s", host, serv);

    Listen(listenfd, LISTENQ);

    if (addrlen) {
        *addrlen = res->ai_addrlen;
    }

    freeaddrinfo(ressave);
    return (listenfd);
}
