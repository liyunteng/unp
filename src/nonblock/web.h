/*
 * web.h - web
 *
 * Date   : 2019/12/11
 */
#ifndef WEB_H
#define WEB_H
#include "unp.h"

#define MAXFILES 20
#define SERV "80"

struct file {
    char *f_name;               /* file name */
    char *f_host;               /* host name or IPv4/IPv6 address */
    int f_fd;                   /* descriptor */
    int f_flags;                /* F_xxx below */
} file[MAXFILES];


#define F_CONNECTING 1          /* connect() in progress */
#define F_READING 2             /* connect() complete; now reading */
#define F_DONE 4                /* all done */

#define GET_CMD "GET %s HTTP/1.0\r\nUser-Agent: curl\r\n\r\n"


int nconn, nfiles, nlefttoconn, nlefttoread, maxfd;
fd_set rset, wset;

void home_page(const char *, const char *);
void start_connect(struct file *);
void write_get_cmd(struct file *);
#endif /* WEB_H */
