/*
 * libp2p.h - libp2p
 *
 * Date   : 2020/04/16
 */
#ifndef LIBP2P_H
#define LIBP2P_H

#include <stdio.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <stdint.h>

#define MAXEVENTS 10
#define BUFFSIZE  4096

#define Debug(format, ...) printf("%s:%d [DEBUG] " format "\n", __FUNCTION__, __LINE__, ##__VA_ARGS__)
#define Info(format, ...)  printf("%s:%d [INFO ] " format "\n", __FUNCTION__, __LINE__, ##__VA_ARGS__)
#define Warn(format, ...)  printf("%s:%d [WARN ] " format "\n", __FUNCTION__, __LINE__,  ##__VA_ARGS__)
#define Error(format, ...) printf("%s:%d [ERROR] " format "\n", __FUNCTION__, __LINE__, ##__VA_ARGS__)

int udp_set_nonblock(int fd);
int udp_client_sockfd(const char *host, const char *service, struct sockaddr **saptr, socklen_t *lenp);
int udp_server_sockfd(const char *host, const char *service, socklen_t *lenp);

/* stun_header_t magic*/
#define STUN_MAGIC  0x2112A422

/* stun_header_t msg_type */
#define STUN_MSG_TYPE_BINDING_REQUEST           0x0001
#define STUN_MSG_TYPE_BINDING_RESPONSE          0x0101
#define STUN_MSG_TYPE_BINDING_ERR_RESPONSE      0x0111
#define STUN_MSG_TYPE_SHARE_SECRET_REQUEST      0x0002
#define STUN_MSG_TYPE_SHARE_SECRET_RESPONSE     0x0102
#define STUN_MSG_TYPE_SHARE_SECRET_ERR_RESPONSE 0x0112

/* stun_attr_header_t type */
#define STUN_ATTR_MAPPED_ADDRESS     0x0001
#define STUN_ATTR_RESPONSE_ADDRESS   0x0002
#define STUN_ATTR_CHANGE_REQUEST     0x0003
#define STUN_ATTR_SOURCE_ADDRESS     0x0004
#define STUN_ATTR_CHANGED_ADDRESS    0x0005
#define STUN_ATTR_USERNAME           0x0006
#define STUN_ATTR_PASSWORD           0x0007
#define STUN_ATTR_MESSAGE_INTEGRITY  0x0008
#define STUN_ATTR_ERROR_CODE         0x0009
#define STUN_ATTR_UNKNOWN_ATTRIBUTES 0x000a
#define STUN_ATTR_REFLECTED_FORM     0x000b
#define STUN_ATTR_REALM              0x0014
#define STUN_ATTR_NONCE              0x0015
#define STUN_ATTR_XOR_MAPPED_ADDRESS 0x0020

#define STUN_ATTR_HEADER_SIZE 4
#define STUN_HEADER_SIZE      20
typedef struct {
    uint16_t type;
    uint16_t length;
} stun_attr_header_t;


typedef struct {
    uint8_t reseved;
    uint8_t family;
    uint16_t port;
    uint32_t address;
} mapped_address_t;

typedef struct {
    uint16_t msg_type;
    uint16_t msg_length;
    uint32_t magic;      /* always be 0x2112A442 */
    uint32_t id_hi;
    uint64_t id_low;
} stun_header_t;

int stun_get_binding_request(uint8_t *buf, size_t *len);
int stun_parse_request(uint8_t *buf, size_t len, char *ip, int *port);

#endif
