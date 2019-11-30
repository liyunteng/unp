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

#endif
