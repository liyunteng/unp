/*
 * strclinonb.c - strclinonb
 *
 * Date   : 2019/12/05
 */
#include "unp.h"

void
str_cli(FILE *fp, int sockfd)
{
    int maxfdp1, val, stdineof;
    ssize_t n, nwriten;
    fd_set rset, wset;
    char to[MAXLINE], fr[MAXLINE];
    char *toiptr, *tooptr, *friptr, *froptr;

    val = Fcntl(sockfd, F_GETFL, 0);
}
