/*
 * libudp.h - libudp
 *
 * Date   : 2019/11/30
 */
#ifndef LIBUDP_H
#define LIBUDP_H

#include "comm.h"
/* UDP */
void dg_echo(int sockfd, SA *pcliaddr, socklen_t clilen);
void dg_cli(FILE *fp, int sockfd, const SA *serveraddr, socklen_t serlen);
void dg_cliconnect(FILE *fp, int sockfd, const SA *serveraddr,
                   socklen_t serlen);
void dg_cli01(FILE *fp, int sockfd, const SA *serveraddr, socklen_t serlen);
void dg_echoloop(int sockfd, SA *pcliaddr, socklen_t clilen);
void dg_cliloop(FILE *fp, int sockfd, const SA *serveraddr, socklen_t serlen);

int udp_client(const char *host, const char *serv, SA **saptr, socklen_t *lenp);
int udp_connect(const char *host, const char *serv);
int udp_server(const char *host, const char *serv, socklen_t *lenp);
#endif
