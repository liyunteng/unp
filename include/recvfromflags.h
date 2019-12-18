/*
 * recvfromflags.h - recvfromflags
 *
 * Date   : 2019/12/18
 */
#ifndef RECVFROMFLAGS_H
#define RECVFROMFLAGS_H
#include "unp.h"

ssize_t
recvfrom_flags(int fd, void *ptr, size_t nbytes, int *flagsp,
               SA *sa, socklen_t *salenptr, struct in_pktinfo *pktp);
void
dg_echo_recvfromflags(int sockfd, SA *pcliaddr, socklen_t clilen);
#endif
