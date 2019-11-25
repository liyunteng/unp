/*
 * unp.h - unp
 *
 * Date   : 2019/11/04
 */

#ifndef UNP_H
#define UNP_H

#include <sys/types.h>
#include <sys/socket.h>
#include <time.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <netdb.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/uio.h>
#include <sys/wait.h>
#include <sys/un.h>
#include <unistd.h>

#include <sys/select.h>
/* #include <sys/sysctl.h> */
#include <poll.h>
#include <strings.h>
#include <sys/ioctl.h>
#include <pthread.h>


#define LISTENQ         1024
#define MAXLINE         4096
#define BUFFSIZE        4096

#define SERV_PORT       9877
#define SERV_PORT_STR   "9877"
#define UNIXSTR_PATH    "/tmp/unix.str"
#define UNIXDG_PATH     "/tmp/unix.dg"

#define FILE_MODE   (S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH)
#define DIR_MODE    (FIILE_MODE | S_IXUSR | S_IXGRP | S_IXOTH)

#define min(a, b)   ((a) < (b) ? (a) : (b))
#define max(a, b)   ((a) > (b) ? (a) : (b))

#ifndef sighandler_t
typedef void (*sighandler_t)(int);
#endif // sighandler_t
typedef struct sockaddr SA;
/* Wrapper */
int Socket(int family, int type, int protocol);
int Connect(int sockfd, const struct sockaddr *addr, socklen_t addrlen);
int Bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen);
int Listen(int sockfd, int backlog);
int Accept(int sockfd, struct sockaddr *addr, socklen_t*addrlen);
int Close(int fd);
ssize_t Write(int fd, const void *buf, size_t count);
ssize_t Read(int fd, void *buf, size_t count);
char *Fgets(char *s, int size, FILE *fp);
int Fputs(const char *s, FILE *fp);
int Inet_pton(int family, const char *ptr, void *buf);
void Pthread_mutex_lock(pthread_mutex_t*mptr);
pid_t Fork(void);
sighandler_t Signal(int signum, sighandler_t handler);
int Select(int nfds, fd_set *readfds, fd_set *writefds,
           fd_set *exceptfds, struct timeval *timeout);
int Poll(struct pollfd *fds, nfds_t nfds, int timeout);
int Shutdown(int fd, int howto);

/* Error Handler */
void err_ret(const char *fmt, ...);
void err_sys(const char *fmt, ...);
void err_dump(const char *fmt, ...);
void err_msg(const char *fmt, ...);
void err_quit(const char *fmt, ...);

ssize_t readn(int fd, void *vptr, size_t n);
ssize_t Readn(int fd, void *vptr, size_t n);
ssize_t writen(int fd, const void *vptr, size_t n);
ssize_t Writen(int fd, const void *vptr, size_t n);
ssize_t readline(int fd, void*vptr, size_t maxlen);
ssize_t Readline(int fd, void*vptr, size_t maxlen);

void str_echo(int sockfd);
void str_echo08(int sockfd);
void str_cli(FILE *fp, int sockfd);
#define str_cli08 str_cli
void bin_echo(int sockfd);
void bin_cli(FILE *fp, int sockfd);
void str_cli_select(FILE *fp, int sockfd);
void str_cli_select02(FILE *fp, int sockfd);
#endif
