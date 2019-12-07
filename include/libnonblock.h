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

#endif
