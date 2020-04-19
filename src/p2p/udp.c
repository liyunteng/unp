/*
 * udp.c - udp
 *
 * Date   : 2020/04/19
 */
#include <arpa/inet.h>
#include <netdb.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <time.h>
#include <assert.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>

#include "udp.h"
#include "log.h"

int
udp_set_nonblock(int fd)
{
    int val, rc;

    val = fcntl(fd, F_GETFL, 0);
    assert(val > 0);
    if (val < 0) {
        Error("fcntl F_GETFL failed: %s", strerror(errno));
        return -1;
    }

    rc = fcntl(fd, F_SETFL, val | O_NONBLOCK);
    assert(rc >= 0);
    if (rc < 0) {
        Error("fcntl F_SETFL failed: %s", strerror(errno));
        return -1;
    }
    return 0;
}

static int
_udp_get_sockfd(const char *host, const char *service, struct addrinfo **ai)
{
    int n;
    struct addrinfo hints;

    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family   = AF_UNSPEC;
    hints.ai_socktype = SOCK_DGRAM;
    if (host != NULL && inet_aton(host, NULL) == 1) {
        hints.ai_flags |= AI_NUMERICSERV;
    }
    if (service != NULL && atoi(service) > 0) {
        hints.ai_flags |= AI_NUMERICSERV;
    }

    if ((n = getaddrinfo(host, service, &hints, ai)) != 0) {
        Error("getaddrinfo for %s:%s failed: %s", host, service,
              gai_strerror(n));
        return -1;
    }
    return 0;
}

int
udp_client_sockfd(const char *host, const char *service,
                  struct sockaddr **saptr, socklen_t *lenp)
{
    int sockfd, rc;
    struct addrinfo *res, *ressave;

    rc = _udp_get_sockfd(host, service, &res);
    if (rc < 0) {
        return rc;
    }
    ressave = res;

    do {
        sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
        if (sockfd >= 0)
            break;
    } while ((res = res->ai_next) != NULL);

    if (res == NULL) {
        Error("no valid address for %s:%s", host, service);
        return -1;
    }

    *saptr = malloc(res->ai_addrlen);
    assert(*saptr != NULL);
    memcpy(*saptr, res->ai_addr, res->ai_addrlen);
    *lenp = res->ai_addrlen;

    freeaddrinfo(ressave);

    int val = 1;
    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &val, sizeof(val)) < 0) {
        Error("setsockopt SO_REUSEADDR failed: %s", strerror(errno));
        return -1;
    }

    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEPORT, &val, sizeof(val)) < 0) {
        Error("setsockopt SO_REUSEPORT failed: %s", strerror(errno));
        return -1;
    }

    return (sockfd);
}

int
udp_server_sockfd(const char *host, const char *service, socklen_t *lenp)
{
    int sockfd, rc;
    struct addrinfo *res, *ressave;

    rc = _udp_get_sockfd(host, service, &res);
    if (rc < 0) {
        return rc;
    }
    ressave = res;

    do {
        sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
        if (sockfd < 0)
            continue;

        /* int on = 1;
         * Setsockopt(sockfd, SOL_SOCKET, SO_BROADCAST, &on, sizeof(on)); */

        if (bind(sockfd, res->ai_addr, res->ai_addrlen) == 0)
            break;

        close(sockfd);
    } while ((res = res->ai_next) != NULL);

    if (res == NULL) {
        Error("no valid address for %s, %s", host, service);
        return -1;
    }

    if (lenp)
        *lenp = res->ai_addrlen;

    int val = 1;
    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &val, sizeof(val)) < 0) {
        Error("setsockopt SO_REUSEADDR failed: %s", strerror(errno));
        return -1;
    }

    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEPORT, &val, sizeof(val)) < 0) {
        Error("setsockopt SO_REUSEPORT failed: %s", strerror(errno));
        return -1;
    }
    return (sockfd);
}
