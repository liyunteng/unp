/*
 * libp2p.c - libp2p
 *
 * Date   : 2020/04/16
 */
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <string.h>
#include <assert.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/epoll.h>
#include <unistd.h>
#include <time.h>

#include "libp2p.h"


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
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_DGRAM;
    if (host != NULL && inet_aton(host, NULL) == 1) {
        hints.ai_flags |= AI_NUMERICSERV;
    }
    if (service != NULL && atoi(service) > 0) {
        hints.ai_flags |= AI_NUMERICSERV;
    }

    if ((n = getaddrinfo(host, service, &hints, ai)) != 0) {
        Error("getaddrinfo for %s:%s failed: %s", host, service, gai_strerror(n));
        return -1;
    }
    return 0;
}

int
udp_client_sockfd(const char *host, const char *service, struct sockaddr **saptr, socklen_t *lenp)
{
    int sockfd, rc;
    struct addrinfo *res, *ressave;

    rc = _udp_get_sockfd(host, service, &res);
    if (rc < 0) {
        return rc;
    }
    ressave = res;

    do  {
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

const char *
stun_msg_type_to_string(stun_msg_type type)
{
    switch(type) {
    case STUN_BINDING_REQUEST:
        return "STUN_BINDING_REQUEST";
    case STUN_SHARE_SECRET_REQUEST:
        return "STUN_SHARE_SECRET_REQUEST";
    case STUN_ALLOCATE:
        return "STUN_ALLOCATE";
    case STUN_REFRESH:
        return "STUN_REFRESH";
    case STUN_CONNECT:
        return "STUN_CONNECT";
    case STUN_IND_SEND:
        return "STUN_IND_SEND";
    case STUN_IND_DATA:
        return "STUN_IND_DATA";
    case STUN_CREATPERMISSION:
        return "STUN_CREATPERMISSION";
    case STUN_CHANNELBIND:
        return "STUN_CHANNELBIND";
    default:
        return "UNKNOWN";
    }
}

int stun_get_binding_request(uint8_t *buf, size_t *len)
{
    if (!buf || !len) {
        return -1;
    }
    unsigned long r;
    stun_header_t binding;
    memset(&binding, 0, sizeof(binding));
    binding.msg_type = htons(STUN_BINDING_REQUEST);
    binding.magic = htonl(STUN_MAGIC);
    binding.msg_length = 0;

    r = 0x0102030405060708;
    binding.id_hi = 0;
    binding.id_low = htobe64(r);
    /* memcpy(binding.id, &r, sizeof(r)); */

    memcpy(buf, &binding, sizeof(binding));
    *len = sizeof(binding);
    return *len;
}

int stun_parse_request(uint8_t* buf,size_t len,char* ip,int* port)
{
    if (!buf || len <= 0 || !ip || !port) {
        Error("invalid argument");
        return -1;
    }

    stun_header_t *header = (stun_header_t *)buf;
    header->msg_type = ntohs(header->msg_type);
    header->msg_length = ntohs(header->msg_length);
    switch (header->msg_type) {
    case STUN_BINDING_REQUEST:
    {
        if (header->msg_length > 0) {
            stun_attr_header_t *attr = (stun_attr_header_t *)(buf + sizeof(stun_header_t));
            attr->type = ntohs(attr->type);
            attr->length = ntohs(attr->length);
            switch (attr->type) {
            case STUN_ATTR_XOR_MAPPED_ADDRESS: {
                if (attr->length > 0) {
                    mapped_address_t *map = (mapped_address_t *)(buf + sizeof(stun_header_t) + sizeof(stun_attr_header_t));
                    map->port = ntohs(map->port);
                    map->port ^= STUN_MAGIC >> 16;
                    map->address = ntohl(map->address);
                    map->address ^= STUN_MAGIC;

                    sprintf(ip, "%d.%d.%d.%d",
                            map->address >> 24,
                            map->address >> 16 & 0xff,
                            map->address >> 8 & 0xff,
                            map->address & 0xff);
                    *port = map->port;
                    return 0;
                }
            }
            case STUN_ATTR_MAPPED_ADDRESS: {
                if (attr->length > 0) {
                    mapped_address_t *map = (mapped_address_t *)(buf + sizeof(stun_header_t) + sizeof(stun_attr_header_t));
                    map->port = ntohs(map->port);
                    map->address = ntohl(map->address);
                    sprintf(ip, "%d.%d.%d.%d",
                            map->address >> 24,
                            map->address >> 16 & 0xff,
                            map->address >> 8 & 0xff,
                            map->address & 0xff);
                    *port = map->port;
                    return 0;
                }
            }
            }
        }
    }
    }

    return -1;
}
