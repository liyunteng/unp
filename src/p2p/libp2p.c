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
stun_msg_type_to_string(uint16_t type)
{
    switch(type) {
    case (STUN_BINDING | (STUN_CLASS_REQUEST << 4)):
        return "STUN_BINDING_REQUEST";
    case (STUN_BINDING | (STUN_CLASS_INDICATION << 4)):
        return "STUN_BINDING_INDICATION";
    case (STUN_BINDING | (STUN_CLASS_RESPONSE << 4)):
        return "STUN_BINDING_RESPONSE";
    case (STUN_BINDING | (STUN_CLASS_ERROR << 4)):
        return "STUN_BINDING_ERROR";

    case (STUN_SHARE_SECRET | (STUN_CLASS_REQUEST << 4)):
        return "STUN_SHARE_SECRET_REQUEST";
    case (STUN_SHARE_SECRET | (STUN_CLASS_INDICATION << 4)):
        return "STUN_SHARE_SECRET_INDICATION";
    case (STUN_SHARE_SECRET | (STUN_CLASS_RESPONSE << 4)):
        return "STUN_SHARE_SECRET_RESPONSE";
    case (STUN_SHARE_SECRET | (STUN_CLASS_ERROR << 4)):
        return "STUN_SHARE_SECRET_ERROR";

    case (STUN_ALLOCATE | (STUN_CLASS_REQUEST << 4)):
        return "STUN_ALLOCATE_REQUEST";
    case (STUN_ALLOCATE | (STUN_CLASS_INDICATION << 4)):
        return "STUN_ALLOCATE_INDICATION";
    case (STUN_ALLOCATE | (STUN_CLASS_RESPONSE << 4)):
        return "STUN_ALLOCATE_RESPONSE";
    case (STUN_ALLOCATE | (STUN_CLASS_ERROR << 4)):
        return "STUN_ALLOCATE_ERROR";

    case (STUN_REFRESH | (STUN_CLASS_REQUEST << 4)):
        return "STUN_REFRESH_REQUEST";
    case (STUN_REFRESH | (STUN_CLASS_INDICATION << 4)):
        return "STUN_REFRESH_INDICATION";
    case (STUN_REFRESH | (STUN_CLASS_RESPONSE << 4)):
        return "STUN_REFRESH_RESPONSE";
    case (STUN_REFRESH | (STUN_CLASS_ERROR << 4)):
        return "STUN_REFRESH_ERROR";

    case (STUN_CONNECT | (STUN_CLASS_REQUEST << 4)):
        return "STUN_CONNECT_REQUEST";
    case (STUN_CONNECT | (STUN_CLASS_INDICATION << 4)):
        return "STUN_CONNECT_INDICATION";
    case (STUN_CONNECT | (STUN_CLASS_RESPONSE << 4)):
        return "STUN_CONNECT_RESPONSE";
    case (STUN_CONNECT | (STUN_CLASS_ERROR << 4)):
        return "STUN_CONNECT_ERROR";

    case (STUN_IND_SEND | (STUN_CLASS_REQUEST << 4)):
        return "STUN_IND_SEND_REQUEST";
    case (STUN_IND_SEND | (STUN_CLASS_INDICATION << 4)):
        return "STUN_IND_SEND_INDICATION";
    case (STUN_IND_SEND | (STUN_CLASS_RESPONSE << 4)):
        return "STUN_IND_SEND_RESPONSE";
    case (STUN_IND_SEND | (STUN_CLASS_ERROR << 4)):
        return "STUN_IND_SEND_ERROR";

    case (STUN_IND_DATA | (STUN_CLASS_REQUEST << 4)):
        return "STUN_IND_DATA_REQUEST";
    case (STUN_IND_DATA | (STUN_CLASS_INDICATION << 4)):
        return "STUN_IND_DATA_INDICATION";
    case (STUN_IND_DATA | (STUN_CLASS_RESPONSE << 4)):
        return "STUN_IND_DATA_RESPONSE";
    case (STUN_IND_DATA | (STUN_CLASS_ERROR << 4)):
        return "STUN_IND_DATA_ERROR";

    case (STUN_CREATPERMISSION | (STUN_CLASS_REQUEST << 4)):
        return "STUN_CREATPERMISSION_REQUEST";
    case (STUN_CREATPERMISSION | (STUN_CLASS_INDICATION << 4)):
        return "STUN_CREATPERMISSION_INDICATION";
    case (STUN_CREATPERMISSION | (STUN_CLASS_RESPONSE << 4)):
        return "STUN_CREATPERMISSION_RESPONSE";
    case (STUN_CREATPERMISSION | (STUN_CLASS_ERROR << 4)):
        return "STUN_CREATPERMISSION_ERROR";

    case (STUN_CHANNELBIND | (STUN_CLASS_REQUEST << 4)):
        return "STUN_CHANNELBIND_REQUEST";
    case (STUN_CHANNELBIND | (STUN_CLASS_INDICATION << 4)):
        return "STUN_CHANNELBIND_INDICATION";
    case (STUN_CHANNELBIND | (STUN_CLASS_RESPONSE << 4)):
        return "STUN_CHANNELBIND_RESPONSE";
    case (STUN_CHANNELBIND | (STUN_CLASS_ERROR << 4)):
        return "STUN_CHANNELBIND_ERROR";

    default:
        return "UNKNOWN";
    }
}

void
stun_make_trans_id(stun_trans_id id)
{
    memset((void *)id, 0, sizeof(stun_trans_id));
    long r = 0x1020304050607080;
    /* srandom((unsigned)time(NULL)); */
    /* long r = random(); */
    memcpy((void *)id, &r, sizeof(r));
}

int stun_trans_id_to_string(stun_trans_id id, char *buf, size_t len)
{
    if (!buf || len < (3 + 2 * STUN_TRANS_ID_SIZE)) {
        return -1;
    }
    memset(buf, 0, len);
    int n;
    n = snprintf(buf, len, "0x");
    for (int i = 0; i < STUN_TRANS_ID_SIZE; i++) {
        n += snprintf(buf+n, len-n, "%02X", id[STUN_TRANS_ID_SIZE - i - 1]);
    }
    return 0;
}

uint16_t stun_get_type(stun_class class,  stun_msg_type type)
{
    uint16_t r = 0;
    r = ((class << 4) & 0xFF0) | (type & 0x000F);
    return r;
}

int stun_get_binding_request(uint8_t *buf, size_t len, stun_trans_id id)
{
    if (!buf || len < STUN_HEADER_SIZE) {
        return -1;
    }

    stun_header_t header;
    memset(&header, 0, STUN_HEADER_SIZE);
    header.msg_type = htons(stun_get_type(STUN_CLASS_REQUEST, STUN_BINDING));
    header.magic = htonl(STUN_MAGIC);
    header.msg_length = 0;
    memcpy(header.id, id, STUN_TRANS_ID_SIZE);
    memcpy(buf, &header, STUN_HEADER_SIZE);
    return STUN_HEADER_SIZE;
}

int stun_get_binding_response(uint8_t *buf, size_t len, stun_trans_id id, struct sockaddr *addr)
{
    /* TODO:  */
    if (!buf || len < (STUN_HEADER_SIZE + sizeof(stun_attr))) {
        return -1;
    }
    stun_header_t header;
    memset(&header, 0, STUN_HEADER_SIZE);
    header.msg_type = htons(stun_get_type(STUN_CLASS_RESPONSE, STUN_BINDING));
    header.magic = htonl(STUN_MAGIC);
    memcpy(header.id, id, STUN_TRANS_ID_SIZE);
    header.msg_length = 0;

    union {
        const struct sockaddr *addr;
        const struct sockaddr_in *in;
        const struct sockaddr_in6 *in6;
    } sa;

    uint8_t family;
    uint16_t port, alen;
    const void *pa;
    uint8_t *ptr;
    sa.addr = addr;
    switch (addr->sa_family) {
    case AF_INET: {
        const struct sockaddr_in *ipv4 = sa.in;
        family = AF_INET;
        port = ipv4->sin_port;
        alen = 4;
        pa = &ipv4->sin_addr;
        break;
    }
    case AF_INET6: {
        const struct sockaddr_in6 *ipv6 = sa.in6;
        family = AF_INET6;
        port = ipv6->sin6_port;
        alen = 16;
        pa = &ipv6->sin6_addr;
        break;
    }
    default:
        Error("invalid address family");
        return -1;
    }

    stun_attr_header_t attr_header;
    attr_header.type = htons(STUN_ATTR_MAPPED_ADDRESS);
    attr_header.length = htons(4 + alen);

    ptr = buf + STUN_HEADER_SIZE + STUN_ATTR_HEADER_SIZE;
    ptr[0] = 0;
    ptr[1] = family;
    memcpy(ptr + 2, &port, 2);
    memcpy(ptr + 4, pa, alen);

    header.msg_length = htons(STUN_ATTR_HEADER_SIZE + 4 + alen);
    memcpy(buf, &header, STUN_HEADER_SIZE);
    memcpy(buf + STUN_HEADER_SIZE, &attr_header, STUN_ATTR_HEADER_SIZE);

    return STUN_ATTR_HEADER_SIZE + 4 + alen + STUN_HEADER_SIZE;
}

int stun_parse_request(uint8_t* buf,size_t len,char* ip,int* port)
{
    /* TODO: */
    if (!buf || len <= 0 || !ip || !port) {
        Error("invalid argument");
        return -1;
    }

    stun_header_t *header = (stun_header_t *)buf;
    char id[64];
    header->msg_type = ntohs(header->msg_type);

    header->msg_length = ntohs(header->msg_length);
    header->magic = ntohl(header->magic);
    stun_trans_id_to_string(header->id,id,sizeof(id));
    switch (header->msg_type) {
    case (STUN_BINDING | (STUN_CLASS_RESPONSE << 4)):
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
