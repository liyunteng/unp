/*
 * getaddrinfo.c - getaddrinfo
 *
 * Date   : 2019/11/27
 */
#include "unp.h"
#include <getopt.h>

static const char *opts         = "PacnNhf:p:t:s:S:";
static const struct option longopts[] = {
    {"passive", no_argument, NULL, 'P'},
    {"all", no_argument, NULL, 'a'},
    {"canonname", no_argument, NULL, 'c'},
    {"numbericHost", no_argument, NULL, 'n'},
    {"numbericService", no_argument, NULL, 'N'},
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
    err_quit("Usage: %s -[PNnach] [-f <family>] "
             "[-p <protocol>] [-t <socktype>] "
             "[-s <host>] [-S <service>]",
             name);
}

void
parse_opt(int argc, char *argv[], struct addrinfo *hints, const char **host,
          const char **service)
{
    int opt, long_idx;
    if (argc == 1) {
        Usage(argv[0]);
    }

    while ((opt = getopt_long(argc, argv, opts, longopts, &long_idx)) > 0) {
        switch (opt) {
        case 'P':
            hints->ai_flags |= AI_PASSIVE;
            break;
        case 'a':
            hints->ai_flags |= AI_ALL;
            break;
        case 'c':
            hints->ai_flags |= AI_CANONNAME;
            break;
        case 'n':
            hints->ai_flags |= AI_NUMERICHOST;
            break;
        case 'N':
            hints->ai_flags |= AI_NUMERICSERV;
            break;
        case 'h':
            Usage(argv[0]);
            break;
        case 'f':
            if (strcmp(optarg, "inet") == 0) {
                hints->ai_family = AF_INET;
            } else if (strcmp(optarg, "inet6") == 0) {
                hints->ai_family = AF_INET6;
            } else {
                err_quit("unsupport family");
            }
            break;
        case 'p':
            if (strcmp(optarg, "tcp") == 0) {
                hints->ai_protocol = IPPROTO_TCP;
            } else if (strcmp(optarg, "udp") == 0) {
                hints->ai_protocol = IPPROTO_UDP;
            } else {
                err_quit("unsupport protocol");
            }
            break;
        case 't':
            if (strcmp(optarg, "stream") == 0) {
                hints->ai_socktype = SOCK_STREAM;
            } else if (strcmp(optarg, "dgram") == 0) {
                hints->ai_socktype = SOCK_DGRAM;
            } else {
                err_quit("unsupport socktype");
            }
            break;
        case 's':
            *host = optarg;
            break;
        case 'S':
            *service = optarg;
            break;
        default:
            Usage(argv[0]);
            break;
        }
    }
}

int
getAddrInfo(int argc, char *argv[])
{
    int ret;
    struct addrinfo hints;
    struct addrinfo *paddrs, *ptr;
    const char *host    = NULL;
    const char *service = NULL;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    parse_opt(argc, argv, &hints, &host, &service);

    printf("host: %s\nservice: %s\n\n", host, service);
    ret = getaddrinfo(host, service, &hints, &paddrs);
    if (ret < 0) {
        printf("getaddrinfo error: %s\n", gai_strerror(ret));
        return ret;
    }

    for (ptr = paddrs; ptr != NULL; ptr = ptr->ai_next) {
        char flags[128] = {0};
        if (ptr->ai_flags & AI_PASSIVE) {
            strcat(flags, " AI_PASSIVE");
        }
        if (ptr->ai_flags & AI_CANONNAME) {
            strcat(flags, " AI_CANONNAME");
        }
        if (ptr->ai_flags & AI_ADDRCONFIG) {
            strcat(flags, " AI_ADDRCONFIG");
        }
        if (ptr->ai_flags & AI_ALL) {
            strcat(flags, " AI_ALL");
        }
        if (ptr->ai_flags & AI_NUMERICHOST) {
            strcat(flags, " AI_NUMERICHOST");
        }
        if (ptr->ai_flags & AI_V4MAPPED) {
            strcat(flags, " AI_V4MAPPED");
        }
        if (ptr->ai_flags & AI_NUMERICSERV) {
            strcat(flags, " AI_NUMERICSERV");
        }
        if (strlen(flags) > 0) {
            printf("flags:%s\n", flags);
        }

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
        case IPPROTO_IP:
            printf("protocal: IPPROTO_IP\n");
            break;
        case IPPROTO_IPV6:
            printf("protocal: IPPROTO_IPV6\n");
            break;
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

int
main(int argc, char *argv[])
{
    getAddrInfo(argc, argv);
    return 0;
}
