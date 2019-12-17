/*
 * broadcast.h - broadcast
 *
 * Date   : 2019/12/16
 */
#ifndef BROADCAST_H
#define BROADCAST_H
#include "unp.h"

void dg_cli_broadcast(FILE *fp, int sockfd, const SA *pservaddr, socklen_t servlen);
void dg_cli_broadcast_pselect(FILE *fp, int sockfd, const SA *pservaddr, socklen_t servlen);
#endif
