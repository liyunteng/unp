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

typedef struct sockaddr SA;
/* Wrapper */
int Socket(int, int, int);
int Bind(int, const struct sockaddr *, socklen_t);
int Listen(int, int);
int Accept(int, struct sockaddr *, socklen_t*);
int Close(int);
ssize_t Write(int fd, const void *, size_t);
ssize_t Read(int fd, void *, size_t);
void Pthread_mutex_lock(pthread_mutex_t*);

/* Error Handler */
void err_ret(const char *fmt, ...);
void err_sys(const char *fmt, ...);
void err_dump(const char *fmt, ...);
void err_msg(const char *fmt, ...);
void err_quit(const char *fmt, ...);
#endif
