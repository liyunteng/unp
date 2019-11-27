/*
 * getaddrinfo.c - getaddrinfo
 *
 * Date   : 2019/11/27
 */
#include "unp.h"
#include <getopt.h>

static const char *opts = "Pachf:p:t:s:S:";
static struct option longopts[] = {
    {"passive", no_argument, NULL, 'P'},
    {"all", no_argument, NULL, 'a'},
    {"canonname", no_argument, NULL, 'c'},
    {"help", no_argument, NULL, 'h'},
    {"family", required_argument, NULL, 'f'},
    {"protocal", required_argument, NULL, 'p'},
    {"socktype", required_argument, NULL, 't'},
    {"host", required_argument, NULL, 's'},
    {"service", required_argument, NULL, 'S'},
    {NULL, 0, 0, 0},
};

void
Usage(const char *name)
{
    err_quit(
        "Usage: %s -[Pach] [-f <family>] [-p <protocol>] [-t <socktype>] [-s <host>] [-S <service>]", name);
}

int
main(int argc, char *argv[])
{
    int              ret, i;
    struct addrinfo  hints;
    struct addrinfo *paddrs, *ptr;
    char             buf[128];
    int              opt, long_idx;
    const char *host = NULL;
        const char *service = NULL;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family    = AF_UNSPEC;
    if (argc == 1) {
        Usage(argv[0]);
    }
    while ((opt = getopt_long(argc, argv, opts, longopts, &long_idx)) > 0) {
        switch (opt) {
        case 'P':
            hints.ai_flags |= AI_PASSIVE;
            break;
        case 'a':
            hints.ai_flags |= AI_ALL;
            break;
        case 'c':
            hints.ai_flags |= AI_CANONNAME;
            break;
        case 'h':
            Usage(argv[0]);
            break;
        case 'f':
            if (strcmp(optarg, "inet") == 0) {
                hints.ai_family = AF_INET;
            } else if (strcmp(optarg, "inet6") == 0) {
                hints.ai_family = AF_INET6;
            } else {
                err_quit("unsupport family");
            }
            break;
        case 'p':
            if (strcmp(optarg, "tcp") == 0) {
                hints.ai_protocol = IPPROTO_TCP;
            } else if (strcmp(optarg, "udp") == 0) {
                hints.ai_protocol = IPPROTO_UDP;
            } else {
                err_quit("unsupport protocol");
            }
            break;
        case 't':
            if (strcmp(optarg, "stream") == 0) {
                hints.ai_socktype = SOCK_STREAM;
            } else if (strcmp(optarg, "dgram") == 0) {
                hints.ai_socktype = SOCK_DGRAM;
            } else {
                err_quit("unsupport socktype");
            }
            break;
        case 's':
            host = optarg;
            break;
        case 'S':
            service = optarg;
            break;
        default:
            Usage(argv[0]);
            break;
        }
    }
    printf("host: %s\nservice: %s\n", host, service);
    ret = getaddrinfo(host, service, &hints, &paddrs);
    if (ret < 0) {
        printf("getaddrinfo error: %s\n", gai_strerror(ret));
        return ret;
    }

    for (ptr = paddrs; ptr != NULL; ptr = ptr->ai_next) {
        printf("falgs: ");
        if (ptr->ai_flags & AI_CANONNAME) {
            printf("AI_CANONNAME ");
        }
        if (ptr->ai_flags & AI_PASSIVE) {
            printf("AI_PASSIVE ");
        }
        if (ptr->ai_flags & AI_ADDRCONFIG) {
            printf("AI_ADDRCONFIG ");
        }
        if (ptr->ai_flags & AI_ALL) {
            printf("AI_ALL ");
        }
        if (ptr->ai_flags & AI_NUMERICHOST) {
            printf("AI_NUMERICHOST ");
            ;
        }
        if (ptr->ai_flags & AI_V4MAPPED) {
            printf("AI_V4MAPPED ");
        }
        if (ptr->ai_flags & AI_NUMERICSERV) {
            printf("AI_NUMERICSERV ");
        }
        printf("\n");
        switch (ptr->ai_family) {
        case AF_INET:
            printf("family: AF_INET\n");
            break;
        case AF_INET6:
            printf("family: AF_INET6\n");
            break;
        default:
            printf("family: %d(unknown)\n", ptr->ai_family);
            break;
        }
        switch (ptr->ai_socktype) {
        case SOCK_DGRAM:
            printf("socktype: SOCK_DGRAM\n");
            break;
        case SOCK_STREAM:
            printf("socktype: SOCK_STREAM\n");
            break;
        case SOCK_RAW:
            printf("socktype: SOCK_RAW\n");
            break;
        case SOCK_SEQPACKET:
            printf("socktype: SOCK_SEQPACKET\n");
            break;
        case SOCK_PACKET:
            printf("socktype: SOCK_PACKET\n");
            break;
        default:
            printf("socktype: %d(unknown)\n", ptr->ai_socktype);
            break;
        }
        switch (ptr->ai_protocol) {
        case IPPROTO_TCP:
            printf("protocal: IPPROTO_TCP\n");
            break;
        case IPPROTO_UDP:
            printf("protocal: IPPROTO_UDP\n");
            break;
        case IPPROTO_SCTP:
            printf("protocal: IPPROTO_SCTP\n");
            break;
        default:
            printf("protocal: %d(unknown)\n", ptr->ai_protocol);
            break;
        }

        printf("addrlen: %d\n", ptr->ai_addrlen);

        printf("address: %s\n", Sock_ntop(ptr->ai_addr, ptr->ai_addrlen));
        if (ptr->ai_canonname) {
            printf("official name: %s\n", ptr->ai_canonname);
        }
        printf("\n");
    }
    freeaddrinfo(paddrs);

    return 0;
}
