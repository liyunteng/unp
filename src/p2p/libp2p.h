/*
 * libp2p.h - libp2p
 *
 * Date   : 2020/04/16
 */
#ifndef LIBP2P_H
#define LIBP2P_H
#include <stdio.h>
#include <stdint.h>
#include <sys/socket.h>

/* stun_header_t magic*/
#define STUN_MAGIC 0x2112A422

#define STUN_MSG_TYPE_POS 0
#define STUN_MSG_TYPE_SIZE 2
#define STUN_MSG_LENGTH_POS (STUN_MSG_TYPE_POS + STUN_MSG_TYPE_SIZE)
#define STUN_MSG_LENGTH_SIZE 2
#define STUN_MSG_MAGIC_POS (STUN_MSG_LENGTH_POS + STUN_MSG_LENGTH_SIZE)
#define STUN_MSG_MAGIC_SIZE 4
#define STUN_MSG_TRANS_ID_POS (STUN_MSG_MAGIC_POS + STUN_MSG_MAGIC_SIZE)
#define STUN_MSG_TRANS_ID_SIZE 12
#define STUN_ATTR_POS (STUN_MSG_TRANS_ID_POS + STUN_MSG_TRANS_ID_SIZE)
#define STUN_MSG_HEADER_SIZE STUN_ATTR_POS
#define STUN_ATTR_TYPE_POS 0
#define STUN_ATTR_TYPE_SIZE 2
#define STUN_ATTR_LENGTH_POS (STUN_ATTR_TYPE_POS + STUN_ATTR_TYPE_SIZE)
#define STUN_ATTR_LENGTH_SIZE 2
#define STUN_ATTR_VALUE_POS (STUN_ATTR_LENGTH_POS + STUN_ATTR_LENGTH_SIZE)
#define STUN_ATTR_HEADER_SIZE STUN_ATTR_VALUE_POS


typedef enum {
    STUN_CLASS_REQUEST    = 0x00,
    STUN_CLASS_INDICATION = 0x01,
    STUN_CLASS_RESPONSE   = 0x10,
    STUN_CLASS_ERROR      = 0x11,
} stun_msg_class;

/* stun_header_t msg_type */
typedef enum {
    STUN_BINDING              = 0x0001, /* RFC5389 */
    STUN_SHARE_SECRET         = 0x0002, /* old RFC3489 */
    STUN_ALLOCATE             = 0x0003, /* TURN-12 */
    STUN_SET_ACTIVE_DST       = 0x0004, /* TURN-04 */
    STUN_REFRESH              = 0x0004, /* TURN-12 */
    STUN_SEND                 = 0x0004, /* TURN-00 */
    STUN_CONNECT              = 0x0005, /* TURN-04 */
    STUN_OLD_SET_ACTIVE_DST   = 0x0006, /* TURN-00 */
    STUN_IND_SEND             = 0x0006, /* TURN-12 */
    STUN_IND_DATA             = 0x0007, /* TURN-12 */
    STUN_IND_CONNECT_STATUS   = 0x0008, /* TURN-04 */
    STUN_CREATPERMISSION      = 0x0008, /* TURN-12 */
    STUN_CHANNELBIND          = 0x0009, /* TURN-12 */
} stun_msg_type;

/* stun_attr_header_t type */
typedef enum {
    STUN_ATTR_MAPPED_ADDRESS         = 0x0001, /* RFC5389 */
    STUN_ATTR_RESPONSE_ADDRESS       = 0x0002, /* old RFC3489 */
    STUN_ATTR_CHANGE_REQUEST         = 0x0003, /* old RFC3489 */
    STUN_ATTR_SOURCE_ADDRESS         = 0x0004, /* old RFC3489 */
    STUN_ATTR_CHANGED_ADDRESS        = 0x0005, /* old RFC3489 */
    STUN_ATTR_USERNAME               = 0x0006, /* RFC5389 */
    STUN_ATTR_PASSWORD               = 0x0007, /* old RFC3489 */
    STUN_ATTR_MESSAGE_INTEGRITY      = 0x0008, /* RFC5389 */
    STUN_ATTR_ERROR_CODE             = 0x0009, /* RFC5389 */
    STUN_ATTR_UNKNOWN_ATTRIBUTES     = 0x000A, /* RFC5389 */
    STUN_ATTR_REFLECTED_FORM         = 0x000B, /* old RFC3489 */
    STUN_ATTR_CHANNEL_NUMBER         = 0x000C, /* TURN-12 */
    STUN_ATTR_LIFETIME               = 0x000D, /* TURN-12 */
    STUN_ATTR_MS_ALTERNATE_SERVER    = 0x000E, /* MS-TURN */
    STUN_ATTR_MAGIC_COOKIE           = 0x000F, /* midcom-TURN 08 */
    STUN_ATTR_BANDWIDTH              = 0x0010, /* TURN--04 */
    STUN_ATTR_DESTINATION_ADDRESS    = 0x0011, /* midcom-TURN 08 */
    STUN_ATTR_REMOTE_ADDRESS         = 0x0012, /* TURN-04 */
    STUN_ATTR_PEER_ADDRESS           = 0x0012, /* TURN-09 */
    STUN_ATTR_XOR_PEER_ADDRESS       = 0x0012, /* TURN-12 */
    STUN_ATTR_DATA                   = 0x0013, /* TURN-12 */
    STUN_ATTR_REALM                  = 0x0014, /* RFC5389 */
    STUN_ATTR_NONCE                  = 0x0015, /* RFC5389 */
    STUN_ATTR_RELAY_ADDRESS          = 0x0016, /* TURN-04 */
    STUN_ATTR_RELAYED_ADDRESS        = 0x0016, /* TURN-09 */
    STUN_ATTR_XOR_RELAYED_ADDRESS    = 0x0016, /* TURN-12 */
    STUN_ATTR_REQUESTED_ADDRESS_TYPE = 0x0017, /* TURN-IPv6-05 */
    STUN_ATTR_REQUESTED_PORT_PROPS   = 0x0018, /* TURN-04 */
    STUN_ATTR_REQUESTED_PROPS        = 0x0018, /* TURN-09 */
    STUN_ATTR_EVEN_PORT              = 0x0018, /* TURN-12 */
    STUN_ATTR_REQUESTED_TRANSPORT    = 0x0019, /* TURN-12 */
    STUN_ATTR_DONT_FRAGMENT          = 0x001A, /* TURN-12 */
    /* 0x001B - 0x001F */
    STUN_ATTR_XOR_MAPPED_ADDRESS     = 0x0020, /* RFC5389 */
    STUN_ATTR_TIME_VAL               = 0x0021, /* TURN-04 */
    STUN_ATTR_REQUESTED_IP           = 0x0022, /* TURN-04 */
    STUN_ATTR_RESERVATION_TOKEN      = 0x0022, /* TURN-09 */
    STUN_ATTR_CONNECT_STAT           = 0x0023, /* TURN-04 */
    STUN_ATTR_PRIORITY               = 0x0024, /* ICE-19 */
    STUN_ATTR_USE_CANDIDATE          = 0x0025, /* ICE-19 */
    /* 0x0026 - 0x7FFF */
    /* Optional Attr */
    STUN_ATTR_OPTIONS                = 0x8001, /* libjingle */
    STUN_ATTR_MS_VERSION             = 0x8008, /* MS-TURN */
    STUN_ATTR_MS_XOR_MAPPED_ADDRES   = 0x8020, /* MS-TURN */
    STUN_ATTR_SOFTWARE               = 0x8022, /* RFC5389 */
    STUN_ATTR_ALTERNATE_SERVER       = 0x8023, /* RFC5389 */
    /* 0x8024 - 0x8027 */
    STUN_ATTR_FINGERPRINT            = 0x8028, /* RFC5389 */
    STUN_ATTR_ICE_CONTROLLED         = 0x8029, /* ICE-19 */
    STUN_ATTR_ICE_CONTROLLING        = 0x802A, /* ICE-19 */
    /* 0x802B - 0x804F */
    STUN_ATTR_MS_SEQUENCE_NUMBER     = 0x8050, /* MS-TURN */
    /* 0x8051 - 0x8053 */
    STUN_ATTR_CANDIDATE_IDENTIFIER   = 0x8054, /* MS-ICE2 */
    /* 0x8055 - 0x806F */
    STUN_ATTR_MS_IMPLEMENTATION_VERSION = 0x8070, /* MS-ICE2 */
    /* 0x8071 - 0xC000 */
    STUN_ATTR_NOMINATION = 0xC001,
    STUN_ATTR_/* 0xC002 - 0xFFFF */
} stun_attr_type;


typedef enum {
    STUN_ERROR_TRY_ALTERNATE = 300, /* RFC5389 */
    STUN_ERROR_BAD_REQUEST = 400,   /* RFC5389 */
    STUN_ERROR_UNAUTHORIZED = 401,  /* RFC5389 */
    STUN_ERROR_UNKNOWN_ATTR = 420,  /* RFC5389 */
    STUN_ERROR_ALLOCATION_MISMATCH = 437, /* TURN-12 */
    STUN_ERROR_STALE_NONCE = 438,         /* RFC5389 */
    STUN_ERROR_ACT_DST_ALREADY = 439,     /* TURN-04 */
    STUN_ERROR_UNSUPPORTED_FAMILY = 440,         /* TURN-IPv6-05 */
    STUN_ERROR_WRONG_CREDENTIALS = 441,           /* TURN-12 */
    STUN_ERROR_UNSUPPORTED_TRANSPORT = 442,       /* TURN-12 */
    STUN_ERROR_INVALID_IP = 443,                 /* TURN-04 */
    STUN_ERROR_INVALID_PORT = 444,               /* TURN-04 */
    STUN_ERROR_OP_TCP_ONLY = 445,                /* TURN-04 */
    STUN_ERROR_CONN_ALREADY = 446,               /* TURN-04 */
    STUN_ERROR_ALLOCATION_QUOTA_REACHED = 486,   /* TURN-12 */
    STUN_ERROR_ROLE_CONFLICT = 487,              /* ICE-19 */
    STUN_ERROR_SERVER_ERROR = 500,               /* RFC5389 */
    STUN_ERROR_SERVER_CAPACITY = 507,            /* TURN-04 */
    STUN_ERROR_INSUFFICIENT_CAPACITY = 508,      /* TURN-12 */
    STUN_ERROR_MAX = 699
} stun_error;

typedef enum {
    STUN_RETURN_SUCCESS,
    STUN_RETURN_NOT_FOUND,
    STUN_RETURN_INVALID,
    STUN_RETURN_NOT_ENOUGH_SPACE,
    STUN_RETURN_UNSUPPORTED_ADDRESS
} stun_return;

typedef struct {
    uint16_t type;
    uint16_t length;
} stun_attr_header_t;

typedef uint8_t stun_trans_id[STUN_MSG_TRANS_ID_SIZE];
typedef struct {
    uint16_t msg_type;
    uint16_t msg_length;
    uint32_t magic; /* always be 0x2112A422 */
    stun_trans_id id;
} stun_header_t;

const char *stun_strerror(stun_error code);
void *stun_setw(uint8_t *ptr, uint16_t val);
uint16_t stun_getw(const uint8_t *ptr);
const char * stun_msg_type_to_string(uint16_t type);

void stun_make_trans_id(stun_trans_id id);
int stun_trans_id_to_string(stun_trans_id id, char *buf, size_t len);

ssize_t stun_get_binding_request(uint8_t *buf, size_t len, stun_trans_id id);
ssize_t stun_get_binding_response(uint8_t *buf, size_t len, stun_trans_id id, struct sockaddr *addr, socklen_t addrlen);

int stun_parse_response(uint8_t *buf, size_t len);
void stun_msg_dump(const uint8_t *ptr);

#endif
