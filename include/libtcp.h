/*
 * libtcp.h - libtcp
 *
 * Date   : 2019/11/30
 */
#ifndef LIBTCP_H
#define LIBTCP_H
#include "comm.h"

/* TCP */
void str_echo(int sockfd);
void str_echo08(int sockfd);
void str_cli(FILE *fp, int sockfd);
#define str_cli08 str_cli
void bin_echo(int sockfd);
void bin_cli(FILE *fp, int sockfd);
void str_cli_select(FILE *fp, int sockfd);
void str_cli_select02(FILE *fp, int sockfd);
int tcp_connect(const char *host, const char *service);
int tcp_listen(const char *host, const char *service, socklen_t *addrlen);

#endif
