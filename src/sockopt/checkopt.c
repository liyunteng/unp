/*
 * checkopt.c - checkopt
 *
 * Date   : 2019/11/25
 */
#include "unp.h"
#include <netinet/tcp.h>
#define IPV6 1
union val {
    int i_val;
    long l_val;
    struct linger linger_val;
    struct timeval timeval_val;
} val;

static char *sock_str_flag(union val *, int);
static char *sock_str_int(union val *, int);
static char *sock_str_linger(union val *, int);
static char *sock_str_timeval(union val *, int);

struct sock_opts {
    const char *opt_str;
    int opt_level;
    int opt_name;
    char *(*opt_val_str)(union val *, int);
} sock_opts[] = {
    {"SO_BROADCAST", SOL_SOCKET, SO_BROADCAST, sock_str_flag},
    {"SO_DEBUG", SOL_SOCKET, SO_DEBUG, sock_str_flag},
    {"SO_DONTROUTE", SOL_SOCKET, SO_DONTROUTE, sock_str_flag},
    {"SO_ERROR", SOL_SOCKET, SO_ERROR, sock_str_int},
    {"SO_KEEPALIVE", SOL_SOCKET, SO_KEEPALIVE, sock_str_flag},
    {"SO_LINGER", SOL_SOCKET, SO_LINGER, sock_str_linger},
    {"SO_OOBINLINE", SOL_SOCKET, SO_OOBINLINE, sock_str_flag},
    {"SO_RCVBUF", SOL_SOCKET, SO_RCVBUF, sock_str_int},
    {"SO_SNDBUF", SOL_SOCKET, SO_SNDBUF, sock_str_int},
    {"SO_RCVLOWAT", SOL_SOCKET, SO_RCVLOWAT, sock_str_int},
    {"SO_SNDLOWAT", SOL_SOCKET, SO_SNDLOWAT, sock_str_int},
    {"SO_RCVTIMEO", SOL_SOCKET, SO_RCVTIMEO, sock_str_timeval},
    {"SO_SNDTIMEO", SOL_SOCKET, SO_SNDTIMEO, sock_str_timeval},
    {"SO_REUSEADDR", SOL_SOCKET, SO_REUSEADDR, sock_str_flag},
#ifdef SO_REUSEPORT
    {"SO_REUSEPORT", SOL_SOCKET, SO_REUSEPORT, sock_str_flag},
#else

    {"SO_REUSEPORT", 0, 0, NULL},
#endif  // SO_REUSEPORT

    {"SO_TYPE", SOL_SOCKET, SO_TYPE, sock_str_int},
#ifdef SO_USELOOPBACK
    {"SO_USELOOPBACK", SOL_SOCKET, SO_USELOOPBACK, sock_str_flag},
#else
    {"SO_USELOOPBACK", 0, 0, NULL},
#endif  // SO_USELOOPBACK

    {"IP_TOS", IPPROTO_IP, IP_TOS, sock_str_int},
    {"IP_TTL", IPPROTO_IP, IP_TTL, sock_str_int},
    {"IPV6_DONTFRAG", IPPROTO_IPV6, IPV6_DONTFRAG, sock_str_flag},
    {"IPV6_UNICAST_HOPS", IPPROTO_IPV6, IPV6_UNICAST_HOPS, sock_str_int},
    {"IPV6_V6ONLY", IPPROTO_IPV6, IPV6_V6ONLY, sock_str_flag},

    {"TCP_MAXSEG", IPPROTO_TCP, TCP_MAXSEG, sock_str_int},
    {"TCP_NODELAY", IPPROTO_TCP, TCP_NODELAY, sock_str_flag},
    {NULL, 0, 0, NULL}};
static char buf[128];
static char *
sock_str_flag(union val *ptr, int len)
{
    memset(buf, 0, sizeof(buf));
    if (len != sizeof(int)) {
        snprintf(buf, sizeof(buf), "size (%d) not sizeof(int)", len);
    } else {
        snprintf(buf, sizeof(buf), "%s", (ptr->i_val == 0) ? "off" : "on");
    }
    return buf;
}

static char *
sock_str_int(union val *ptr, int len)
{
    memset(buf, 0, sizeof(buf));
    if (len != sizeof(int)) {
        snprintf(buf, sizeof(buf), "size (%d) not sizeof(int)", len);
    } else {
        snprintf(buf, sizeof(buf), "%d", ptr->i_val);
    }
    return buf;
}

static char *
sock_str_linger(union val *ptr, int len)
{
    memset(buf, 0, sizeof(buf));
    if (len != sizeof(struct linger)) {
        snprintf(buf, sizeof(buf), "size (%d) not sizeof(linger)", len);
    } else {
        snprintf(buf, sizeof(buf), "l_onoff = %d, l_linger = %d",
                 ptr->linger_val.l_onoff, ptr->linger_val.l_linger);
    }

    return buf;
}

static char *
sock_str_timeval(union val *ptr, int len)
{
    memset(buf, 0, sizeof(buf));
    if (len != sizeof(struct timeval)) {
        snprintf(buf, sizeof(buf), "size (%d) not sizeof(timeval)", len);
    } else {
        snprintf(buf, sizeof(buf), "%ld sec, %ld usec", ptr->timeval_val.tv_sec,
                 ptr->timeval_val.tv_usec);
    }
    return buf;
}

int
main(void)
{
    int fd;
    socklen_t len;
    struct sock_opts *ptr;

    for (ptr = sock_opts; ptr->opt_str != NULL; ptr++) {
        printf("%s: ", ptr->opt_str);
        if (ptr->opt_val_str == NULL) {
            printf("(undefined)\n");
        } else {
            switch (ptr->opt_level) {
            case SOL_SOCKET:
            case IPPROTO_IP:
            case IPPROTO_TCP:
                fd = Socket(AF_INET, SOCK_STREAM, 0);
                break;
#ifdef IPV6
            case IPPROTO_IPV6:
                fd = Socket(AF_INET6, SOCK_STREAM, 0);
                break;
#endif
            default:
                err_quit("Can't create fd for level %d\n", ptr->opt_level);
            }

            len = sizeof(val);
            if (getsockopt(fd, ptr->opt_level, ptr->opt_name, &val, &len)
                == -1) {
                err_ret("getsockopt error");
            } else {
                printf("default = %s\n", (*ptr->opt_val_str)(&val, len));
            }
            close(fd);
        }
    }

    return 0;
}
