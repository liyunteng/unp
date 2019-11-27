/*
 * libfree.c - libfree
 *
 * Date   : 2019/11/05
 */

#include "unp.h"

static int
my_inet_pton(int family, const char *strptr, void *addrptr)
{
    if (family == AF_INET) {
        struct in_addr in_val;

        if (inet_aton(strptr, &in_val)) {
            memcpy(addrptr, &in_val, sizeof(struct in_addr));
            return (1);
        }
        return 0;
    }
    errno = EAFNOSUPPORT;
    return (-1);
}

static const char *
my_inet_ntop(int family, const void *addrptr, char *strptr, size_t len)
{
    const u_char *p = (const u_char *)addrptr;

    if (family == AF_INET) {
        char temp[INET_ADDRSTRLEN];

        snprintf(temp, sizeof(temp), "%d.%d.%d.%d", p[0], p[1], p[2], p[3]);
        if (strlen(temp) >= len) {
            errno = ENOSPC;
            return (NULL);
        }
        strcpy(strptr, temp);
        return strptr;
    }
    errno = EAFNOSUPPORT;
    return (NULL);
}

char *
sock_ntop(const struct sockaddr *sa, socklen_t salen)
{
    char portstr[8];
    static char str[128];

    switch (sa->sa_family) {
    case AF_INET: {
        struct sockaddr_in *sin = (struct sockaddr_in *)sa;
        if (inet_ntop(AF_INET, &sin->sin_addr, str, sizeof(str)) == NULL) {
            return (NULL);
        }
        if (ntohs(sin->sin_port) != 0) {
            snprintf(portstr, sizeof(portstr), ":%d", ntohs(sin->sin_port));
            strcat(str, portstr);
        }
        return (str);
    }
    case AF_INET6: {
        struct sockaddr_in6 *sin = (struct sockaddr_in6 *)sa;
        if (inet_ntop(AF_INET6, &sin->sin6_addr, str, sizeof(str)) == NULL) {
            return (NULL);
        }
        if (ntohs(sin->sin6_port) != 0) {
            snprintf(portstr, sizeof(portstr), ":%d", ntohs(sin->sin6_port));
            strcat(str, portstr);
        }
        return (str);
    }
        /* TODO: */
    }
    return (NULL);
}

char *
Sock_ntop(const struct sockaddr *sa, socklen_t salen)
{
    char *n = NULL;
    if ((n = sock_ntop(sa, salen)) == NULL) {
        err_sys("sock_ntop error");
    }
    return n;
}

ssize_t
readn(int fd, void *vptr, size_t n)
{
    size_t nleft;
    ssize_t nread;
    char *ptr;

    ptr   = vptr;
    nleft = n;
    while (nleft > 0) {
        if ((nread = read(fd, ptr, nleft)) < 0) {
            if (errno == EINTR)
                nread = 0;
            else
                return (-1);
        } else if (nread == 0) {
            break;
        }
        nleft -= nread;
        ptr += nread;
    }
    return (n - nleft);
}

ssize_t
Readn(int fd, void *vptr, size_t len)
{
    ssize_t n;
    if ((n = readn(fd, vptr, len)) < 0) {
        err_sys("readn error");
    }
    return n;
}

ssize_t
writen(int fd, const void *vptr, size_t n)
{
    size_t nleft;
    ssize_t nwritten;
    const char *ptr;

    ptr   = vptr;
    nleft = n;
    while (nleft > 0) {
        if ((nwritten = write(fd, ptr, nleft)) <= 0) {
            if (nwritten < 0 && errno == EINTR)
                nwritten = 0;
            else
                return (-1);
        }
        nleft -= nwritten;
        ptr += nwritten;
    }
    return (n);
}
ssize_t
Writen(int fd, const void *vptr, size_t len)
{
    ssize_t n;
    if ((n = writen(fd, vptr, len)) < 0) {
        err_sys("writen error");
    }
    return n;
}

static int read_cnt;
static char *read_ptr;
static char read_buf[MAXLINE];
static ssize_t
my_read(int fd, char *ptr)
{
    if (read_cnt <= 0) {
    again:
        if ((read_cnt = read(fd, read_buf, sizeof(read_buf))) < 0) {
            if (errno == EINTR)
                goto again;
            return (-1);
        } else if (read_cnt == 0)
            return (0);
        read_ptr = read_buf;
    }
    read_cnt--;
    *ptr = *read_ptr++;
    return (1);
}

ssize_t
readline(int fd, void *vptr, size_t maxlen)
{
    ssize_t n, rc;
    char c, *ptr;

    ptr = vptr;
    for (n = 1; n < maxlen; n++) {
        if ((rc = my_read(fd, &c)) == 1) {
            *ptr++ = c;
            if (c == '\n')
                break;
        } else if (rc == 0) {
            *ptr = 0;
            return (n - 1);
        } else {
            return (-1);
        }
    }
    *ptr = 0;
    return (n);
}

ssize_t
Readline(int fd, void *vptr, size_t maxlen)
{
    ssize_t n;
    if ((n = readline(fd, vptr, maxlen)) < 0) {
        err_sys("readline error");
    }
    return n;
}

ssize_t
readlinebuf(void **vptrptr)
{
    if (read_cnt)
        *vptrptr = read_ptr;
    return (read_cnt);
}

int
sockfd_to_family(int sockfd)
{
    struct sockaddr_storage ss;
    socklen_t len;

    len = sizeof(ss);
    if (getsockname(sockfd, (SA *)&ss, &len) < 0) {
        return (-1);
    }
    return (ss.ss_family);
}

typedef void Sigfunc(int);
static Sigfunc *
my_signal(int signo, Sigfunc *func)
{
    struct sigaction act, oact;
    act.sa_handler = func;
    sigemptyset(&act.sa_mask);
    act.sa_flags = 0;
    if (signo == SIGALRM) {
#ifdef SA_INTERRUPT
        act.sa_flags |= SA_INTERRUPT;
#endif  // SA_INTERRUPT
    } else {
#ifdef SA_RESTART
        act.sa_flags |= SA_RESTART;
#endif  // SA_RESTART
    }

    if (sigaction(signo, &act, &oact) < 0)
        return (SIG_ERR);
    return (oact.sa_handler);
}
