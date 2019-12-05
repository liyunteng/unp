/*
 * wrapper.h - wrapper
 *
 * Date   : 2019/11/30
 */
#ifndef WRAPPER_H
#define WRAPPER_H
#include "comm.h"

/* Wrapper */
int Socket(int family, int type, int protocol);
int Connect(int sockfd, const struct sockaddr *addr, socklen_t addrlen);
int Bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen);
int Listen(int sockfd, int backlog);
int Accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen);
int Close(int fd);
ssize_t Write(int fd, const void *buf, size_t count);
ssize_t Read(int fd, void *buf, size_t count);
char *Fgets(char *s, int size, FILE *fp);
int Fputs(const char *s, FILE *fp);
int Inet_pton(int family, const char *ptr, void *buf);
const char *Inet_ntop(int af, const void *src, char *dst, socklen_t size);
void *Malloc(size_t len);
void Pthread_mutex_lock(pthread_mutex_t *mptr);
pid_t Fork(void);
sighandler_t Signal(int signum, sighandler_t handler);
int Sigaction(int sig, const struct sigaction *restrict act,
              struct sigaction *restrict oact);
int Select(int nfds, fd_set *readfds, fd_set *writefds, fd_set *exceptfds,
           struct timeval *timeout);
int Poll(struct pollfd *fds, nfds_t nfds, int timeout);
int Shutdown(int fd, int howto);
ssize_t Recvfrom(int sockfd, void *buf, size_t len, int flags,
                 struct sockaddr *src_addr, socklen_t *addrlen);
ssize_t Sendto(int sockfd, const void *buf, size_t lne, int flags,
               const struct sockaddr *dst_addr, socklen_t addrlen);
char *Sock_ntop(const struct sockaddr *sa, socklen_t salen);
int Getsockname(int sockfd, struct sockaddr *address, socklen_t *len);
int Setsockopt(int sockfd, int level, int optname, const void *optval,
               socklen_t optlen);
int Getsockopt(int sockfd, int level, int optname, void *optval,
               socklen_t *optlen);
int Getpeername(int sockfd, struct sockaddr *addr, socklen_t *addrlen);

int Fcntl(int fd, int cmd, ...);
#endif
