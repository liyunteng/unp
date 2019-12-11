/*
 * libnonblock.h - libnonblock
 *
 * Date   : 2019/12/07
 */
#ifndef LIBNONBLOCK_H
#define LIBNONBLOCK_H
#include "comm.h"

char *gf_time(void);
void str_cli_nonblock(FILE *fp, int sockfd);
void str_cli_nonblock_fork(FILE *fp, int sockfd);

int connect_nonblock(int sockfd, const SA *saptr, socklen_t salen, int nsec);
#endif
