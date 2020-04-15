/*
 * libp2p.h - libp2p
 *
 * Date   : 2020/04/16
 */
#ifndef LIBP2P_H
#define LIBP2P_H

#include <stdio.h>
#include <sys/epoll.h>
#include <sys/socket.h>

#define MAXEVENTS 10
#define BUFFSIZE  4096

#define Debug(format, ...) printf("%s:%d [DEBUG] " format "\n", __FUNCTION__, __LINE__, ##__VA_ARGS__)
#define Info(format, ...)  printf("%s:%d [INFO ] " format "\n", __FUNCTION__, __LINE__, ##__VA_ARGS__)
#define Warn(format, ...)  printf("%s:%d [WARN ] " format "\n", __FUNCTION__, __LINE__,  ##__VA_ARGS__)
#define Error(format, ...) printf("%s:%d [ERROR] " format "\n", __FUNCTION__, __LINE__, ##__VA_ARGS__)

int udp_set_nonblock(int fd);
int udp_client_sockfd(const char *host, const char *service, struct sockaddr **saptr, socklen_t *lenp);
int udp_server_sockfd(const char *host, const char *service, socklen_t *lenp);
#endif
