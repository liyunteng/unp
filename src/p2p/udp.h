/*
 * udp.h - udp
 *
 * Date   : 2020/04/19
 */
#ifndef UDP_H
#define UDP_H
#include <sys/socket.h>

#define MAXEVENTS 10
#define BUFFSIZE 4096


int udp_set_nonblock(int fd);
int udp_client_sockfd(const char *host, const char *service,
                      struct sockaddr **saptr, socklen_t *lenp);
int udp_server_sockfd(const char *host, const char *service, socklen_t *lenp);


#endif
