/*
 * wrapper.c - wrapper
 *
 * Date   : 2019/11/04
 */
#include "unp.h"

int
Socket(int family, int type, int protocol)
{
    int n;
    if ((n = socket(family, type, protocol)) < 0) {
        err_sys("socket error");
    }
    return n;
}

int
Connect(int sockfd, const struct sockaddr *addr, socklen_t addrlen)
{
    int n;
    if ((n = connect(sockfd, addr, addrlen)) < 0) {
        err_sys("connect error");
    }
    return n;
}

int
Bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen)
{
    int n;
    if ((n = bind(sockfd, addr, addrlen)) < 0) {
        err_sys("bind error");
    }
    return n;
}

int
Listen(int sockfd, int backlog)
{
    int n;
    if ((n = listen(sockfd, backlog)) < 0) {
        err_sys("listen error");
    }
    return n;
}

int
Accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen)
{
    int n;
    if ((n = accept(sockfd, addr, addrlen)) < 0) {
        err_sys("accept error");
    }
    return n;
}

int
Inet_pton(int family, const char *ptr, void *buf)
{
    int n;
    if ((n = inet_pton(family, ptr, buf)) != 1) {
        err_sys("inet_pton error");
    }
    return n;
}

const char *
Inet_ntop(int af, const void *src, char *dst, socklen_t size)
{
    const char *n;
    if ((n = inet_ntop(af, src, dst, size)) == NULL) {
        err_sys("inet_ntop error");
    }
    return n;
}

int
Close(int fd)
{
    int n;
    if ((n = close(fd)) < 0) {
        err_sys("close error");
    }
    return n;
}

ssize_t
Write(int fd, const void *buf, size_t count)
{
    ssize_t n;
    if ((n = write(fd, buf, count)) != count) {
        err_sys("write error");
    }
    return n;
}

ssize_t
Read(int fd, void *buf, size_t count)
{
    ssize_t n;
    if ((n = read(fd, buf, count)) < 0) {
        err_sys("read error");
    }
    return n;
}

char *
Fgets(char *s, int size, FILE *fp)
{
    char *n;
    if ((n = fgets(s, size, fp)) == NULL) {
    }
    return n;
}

int
Fputs(const char *s, FILE *fp)
{
    int n;
    if ((n = fputs(s, fp)) == EOF) {
        err_sys("fputs error");
    }
    return n;
}

pid_t
Fork(void)
{
    pid_t n;
    if ((n = fork()) < 0) {
        err_sys("fork error");
    }
    return n;
}

sighandler_t
Signal(int signum, sighandler_t handler)
{
    sighandler_t r = NULL;
    if ((r = signal(signum, handler)) == SIG_ERR) {
        err_sys("singal error");
    }
    return r;
}

int
Sigaction(int sig, const struct sigaction *restrict act,
          struct sigaction *restrict oact)
{
    int n;
    if ((n = sigaction(sig, act, oact)) != 0) {
        err_sys("sigaction error");
    }
    return n;
}

int Sigemptyset(sigset_t *set)
{
    int n;
    if ((n = sigemptyset(set)) != 0) {
        err_sys("sigemptyset error");
    }
    return n;
}

int Sigfillset(sigset_t *set)
{
    int n;
    if ((n = sigfillset(set)) != 0) {
        err_sys("sigfillset error");
    }
    return n;
}

int Sigaddset(sigset_t *set, int signum)
{
    int n;
    if ((n = sigaddset(set, signum)) != 0) {
        err_sys("sigaddset error");
    }
    return n;
}

int Sigdelset(sigset_t *set, int signum)
{
    int n;
    if ((n = sigdelset(set, signum)) != 0) {
        err_sys("sigdelset error");
    }
    return n;
}


int Sigismember(const sigset_t *set, int signum)
{
    int n;
    if ((n = sigismember(set, signum)) != 0) {
        err_sys("sigismember error");
    }
    return n;
}

int Sigprocmask(int how, const sigset_t *set, sigset_t *oset)
{
    int n;
    if ((n = sigprocmask(how, set, oset)) != 0) {
        err_sys("sigprocmask error");
    }
    return n;
}

int
Select(int nfds, fd_set *readfds, fd_set *writefds, fd_set *exceptfds,
       struct timeval *timeout)
{
    int n;
    if ((n = select(nfds, readfds, writefds, exceptfds, timeout)) < 0) {
        err_sys("select error");
    }
    return n;
}

int
Poll(struct pollfd *fds, nfds_t nfds, int timeout)
{
    int n;
    if ((n = poll(fds, nfds, timeout)) < 0) {
        err_sys("poll error");
    }
    return n;
}

int
Shutdown(int fd, int howto)
{
    int n;
    if ((n = shutdown(fd, howto)) < 0) {
        err_sys("shutdown error");
    }
    return n;
}

ssize_t
Recvfrom(int sockfd, void *buf, size_t len, int flags,
         struct sockaddr *src_addr, socklen_t *addrlen)
{
    ssize_t n;
    if ((n = recvfrom(sockfd, buf, len, flags, src_addr, addrlen)) < 0) {
        err_sys("recvfrom error");
    }
    return n;
}

ssize_t
Sendto(int sockfd, const void *buf, size_t len, int flags,
       const struct sockaddr *dst_addr, socklen_t addrlen)
{
    ssize_t n;
    if ((n = sendto(sockfd, buf, len, flags, dst_addr, addrlen)) < 0) {
        err_sys("sendto error");
    }
    return n;
}
void
Pthread_mutex_lock(pthread_mutex_t *mptr)
{
    int n;
    if ((n = pthread_mutex_lock(mptr)) == 0)
        return;
    errno = n;
    err_sys("pthread_mutex_lock error");
}

void *
Malloc(size_t len)
{
    void *n = NULL;
    if ((n = malloc(len)) == NULL) {
        err_sys("malloc error");
    }
    return n;
}

void *
Calloc(size_t nmemb, size_t size)
{
    void *n = NULL;
    if ((n = calloc(nmemb, size)) == NULL) {
        err_sys("calloc error");
    }
    return n;
}


int
Getsockname(int sockfd, struct sockaddr *address, socklen_t *len)
{
    int n;
    if ((n = getsockname(sockfd, address, len)) < 0) {
        err_sys("getsockname error");
    }
    return n;
}

int
Setsockopt(int sockfd, int level, int optname, const void *optval,
           socklen_t optlen)
{
    int n;
    if ((n = setsockopt(sockfd, level, optname, optval, optlen)) < 0) {
        err_sys("setsockopt error");
    }
    return n;
}

int
Getsockopt(int sockfd, int level, int optname, void *optval, socklen_t *optlen)
{
    int n;
    if ((n = getsockopt(sockfd, level, optname, optval, optlen)) < 0) {
        err_sys("getsockopt error");
    }
    return n;
}

int
Getpeername(int sockfd, struct sockaddr *addr, socklen_t *addrlen)
{
    int n;
    if ((n = getpeername(sockfd, addr, addrlen)) != 0) {
        err_sys("getpeername error");
    }
    return n;
}

int
Ioctl(int fd, unsigned long request, void* val)
{
    int n = 0;
    if ((n = ioctl(fd, request, val)) < 0) {
        err_sys("ioctl error");
    }
    return n;
}

int
Fcntl(int fd, int cmd, int val)
{
    int n;
    if ((n = fcntl(fd, cmd, val)) < 0) {
        err_sys("fcntl error");
    }
    return n;
}

int
Gettimeofday(struct timeval *tp, void *tzp)
{
    int n;
    if ((n = gettimeofday(tp, tzp)) != 0) {
        err_sys("gettimeofday error");
    }
    return n;
}
