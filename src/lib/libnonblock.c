/*
 * libnonblock.c - libnonblock
 *
 * Date   : 2019/12/07
 */
#include "unp.h"

char *
gf_time(void)
{
    struct timeval tv;
    static char str[30];
    char *ptr;

    if (gettimeofday(&tv, NULL) < 0) {
        err_sys("gettimeofday error");
    }

    ptr = ctime(&tv.tv_sec);
    strcpy(str, &ptr[11]);
    snprintf(str + 8, sizeof(str) - 8, ".%06ld", tv.tv_usec);
    return (str);
}

void
str_cli_nonblock(FILE *fp, int sockfd)
{
    int maxfdp1, val, stdineof;
    ssize_t n, nwriten;
    fd_set rset, wset;
    char to[MAXLINE], fr[MAXLINE];
    char *toiptr, *tooptr, *friptr, *froptr;

    val = Fcntl(sockfd, F_GETFL, 0);
    Fcntl(sockfd, F_SETFL, val | O_NONBLOCK);

    val = Fcntl(STDIN_FILENO, F_GETFL, 0);
    Fcntl(STDIN_FILENO, F_SETFL, val | O_NONBLOCK);

    val = Fcntl(STDOUT_FILENO, F_GETFL, 0);
    Fcntl(STDOUT_FILENO, F_SETFL, val | O_NONBLOCK);

    toiptr = tooptr = to;
    friptr = froptr = fr;
    stdineof        = 0;

    maxfdp1 = max(max(STDIN_FILENO, STDOUT_FILENO), sockfd) + 1;
    for (;;) {
        FD_ZERO(&rset);
        FD_ZERO(&wset);
        if (stdineof == 0 && toiptr < &to[MAXLINE]) {
            FD_SET(STDIN_FILENO, &rset); /* read from stdin */
        }
        if (friptr < &fr[MAXLINE]) {
            FD_SET(sockfd, &rset); /* read from sock */
        }
        if (tooptr != toiptr) {
            FD_SET(sockfd, &wset); /* data to write to sock */
        }
        if (froptr != friptr) {
            FD_SET(STDOUT_FILENO, &wset); /* data to write to stdout */
        }

        Select(maxfdp1, &rset, &wset, NULL, NULL);
        if (FD_ISSET(STDIN_FILENO, &rset)) {
            if ((n = read(STDIN_FILENO, toiptr, &to[MAXLINE] - toiptr)) < 0) {
                if (errno != EWOULDBLOCK) {
                    err_sys("read error on stdin");
                }
            } else if (n == 0) {
                fprintf(stderr, "%s: EOF on stdin\n", gf_time());
                stdineof = 1;
                if (tooptr == toiptr) {
                    Shutdown(sockfd, SHUT_WR);
                }
            } else {
                fprintf(stderr, "%s: read %ld bytes from stdin\n", gf_time(),
                        n);
                toiptr += n;
                FD_SET(sockfd, &wset);
            }
        }
        if (FD_ISSET(sockfd, &rset)) {
            if ((n = read(sockfd, friptr, &fr[MAXLINE] - friptr)) < 0) {
                if (errno != EWOULDBLOCK) {
                    err_sys("read error on socket");
                }
            } else if (n == 0) {
                fprintf(stderr, "%s: EOF on socket\n", gf_time());
                if (stdineof)
                    return;
                else
                    err_quit("str_cli: server terminated prematurely");
            } else {
                fprintf(stderr, "%s: read %ld bytes from socket\n", gf_time(),
                        n);
                friptr += n;
                FD_SET(STDOUT_FILENO, &wset);
            }
        }
        if (FD_ISSET(STDOUT_FILENO, &wset) && ((n = friptr - froptr) > 0)) {
            if ((nwriten = write(STDOUT_FILENO, froptr, n)) < 0) {
                if (errno != EWOULDBLOCK) {
                    err_sys("write error to stdout");
                }
            } else {
                fprintf(stderr, "%s: wrote %ld bytes to stdout\n", gf_time(),
                        nwriten);
                froptr += nwriten;
                if (froptr == friptr) {
                    froptr = friptr = fr;
                }
            }
        }

        if (FD_ISSET(sockfd, &wset) && ((n = toiptr - tooptr) > 0)) {
            if ((nwriten = write(sockfd, tooptr, n)) < 0) {
                if (errno != EWOULDBLOCK) {
                    err_sys("write error to socket");
                }
            } else {
                fprintf(stderr, "%s: worte %ld bytes to socket\n", gf_time(),
                        nwriten);
                tooptr += nwriten;
                if (tooptr == toiptr) {
                    toiptr = tooptr = to;
                    if (stdineof) {
                        Shutdown(sockfd, SHUT_WR);
                    }
                }
            }
        }
    }
}

void
str_cli_nonblock_fork(FILE *fp, int sockfd)
{
    pid_t pid;
    char sendline[MAXLINE], recvline[MAXLINE];

    if ((pid = fork()) == 0) { /* child */
        while (Readline(sockfd, recvline, MAXLINE) > 0)
            Fputs(recvline, stdout);

        kill(getppid(), SIGTERM); /* in case parent still running */
        exit(0);
    }

    /* parent */
    while (Fgets(sendline, MAXLINE, fp) != 0) {
        Writen(sockfd, sendline, strlen(sendline));
    }

    Shutdown(sockfd, SHUT_WR);
    pause();
    return;
}

int
connect_nonblock(int sockfd, const SA *saptr, socklen_t salen, int nsec)
{
    int flags, n, error;
    socklen_t len;
    fd_set rset, wset;
    struct timeval tv;

    flags = Fcntl(sockfd, F_GETFL, 0);
    Fcntl(sockfd, F_SETFL, flags | O_NONBLOCK);

    error = 0;
    if ((n = connect(sockfd, saptr, salen)) < 0) {
        if (errno != EINPROGRESS) {
            return (-1);
        }
    }

    /* connect completed immediately */
    if (n == 0)
        goto done;

    FD_ZERO(&rset);
    FD_SET(sockfd, &rset);
    wset       = rset;
    tv.tv_sec  = nsec;
    tv.tv_usec = 0;

    if ((n = Select(sockfd + 1, &rset, &wset, NULL, nsec ? &tv : NULL)) == 0) {
        /* timeout */
        close(sockfd);
        errno = ETIMEDOUT;
        return (-1);
    }

    if (FD_ISSET(sockfd, &rset) || FD_ISSET(sockfd, &wset)) {
        len = sizeof(error);
        if (getsockopt(sockfd, SOL_SOCKET, SO_ERROR, &error, &len) < 0) {
            return (-1);
        }
    } else {
        err_quit("select error: sockfd not set");
    }

done:
    Fcntl(sockfd, F_SETFL, flags);
    if (error) {
        close(sockfd);
        errno = error;
        return (-1);
    }
    return 0;
}
