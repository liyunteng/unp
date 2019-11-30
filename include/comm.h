/*
 * comm.h - comm
 *
 * Date   : 2019/11/30
 */
#ifndef COMM_H
#define COMM_H

#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <netdb.h>
#include <netinet/in.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <sys/un.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

#include <sys/select.h>
/* #include <sys/sysctl.h> */
#include <poll.h>
#include <pthread.h>
#include <strings.h>
#include <sys/ioctl.h>

#define LISTENQ 1024
#define MAXLINE 4096
#define BUFFSIZE 4096

#define SERV_PORT 9877
#define SERV_PORT_STR "9877"
#define UNIXSTR_PATH "/tmp/unix.str"
#define UNIXDG_PATH "/tmp/unix.dg"

#define FILE_MODE (S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH)
#define DIR_MODE (FIILE_MODE | S_IXUSR | S_IXGRP | S_IXOTH)

#define min(a, b) ((a) < (b) ? (a) : (b))
#define max(a, b) ((a) > (b) ? (a) : (b))

#ifndef sighandler_t
typedef void (*sighandler_t)(int);
#endif  // sighandler_t
typedef struct sockaddr SA;

ssize_t readn(int fd, void *vptr, size_t n);
ssize_t Readn(int fd, void *vptr, size_t n);
ssize_t writen(int fd, const void *vptr, size_t n);
ssize_t Writen(int fd, const void *vptr, size_t n);
ssize_t readline(int fd, void *vptr, size_t maxlen);
ssize_t Readline(int fd, void *vptr, size_t maxlen);

/* Error Handler */
void err_ret(const char *fmt, ...);
void err_sys(const char *fmt, ...);
void err_dump(const char *fmt, ...);
void err_msg(const char *fmt, ...);
void err_quit(const char *fmt, ...);

#endif
