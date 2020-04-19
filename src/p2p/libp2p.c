/*
 * libp2p.c - libp2p
 *
 * Date   : 2020/04/16
 */
#include <assert.h>
#include <errno.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "libp2p.h"
#include "log.h"


const char *
stun_msg_type_to_string(uint16_t type)
{
    switch (type) {
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

const char *
stun_strerror(stun_error code)
{
    static const struct {
        stun_error code;
        char phrase[32];
    } tab[] = {
        {STUN_ERROR_TRY_ALTERNATE, "Try alternate server"},
        {STUN_ERROR_BAD_REQUEST, "Bad request"},
        {STUN_ERROR_UNAUTHORIZED, "Unauthorized"},
        {STUN_ERROR_UNKNOWN_ATTR, "Unknown Attribute"},
        {STUN_ERROR_ALLOCATION_MISMATCH, "Allocation Mismatch"},
        {STUN_ERROR_STALE_NONCE, "Stale Nonce"},
        {STUN_ERROR_ACT_DST_ALREADY, "Active Destination Already Set"},
        {STUN_ERROR_UNSUPPORTED_FAMILY, "Address Family not Supported"},
        {STUN_ERROR_UNSUPPORTED_TRANSPORT, "Unsupported Transport Protocol"},
        {STUN_ERROR_INVALID_IP, "Invalid IP Address"},
        {STUN_ERROR_INVALID_PORT, "Invalid Port"},
        {STUN_ERROR_OP_TCP_ONLY, "Operation for TCP only"},
        {STUN_ERROR_CONN_ALREADY, "Connection Already Exists"},
        {STUN_ERROR_ALLOCATION_QUOTA_REACHED, "Allocation Quota Reached"},
        {STUN_ERROR_ROLE_CONFLICT, "Role conflict"},
        {STUN_ERROR_SERVER_ERROR, "Server Error"},
        {STUN_ERROR_SERVER_CAPACITY, "Insufficient Capacity"},
        {STUN_ERROR_INSUFFICIENT_CAPACITY, "Insufficient Capacity"},
    };

    const char *str = "Unknown error";
    size_t i;
    for (i = 0; i < (sizeof(tab) / sizeof(tab[0])); i++) {
        if (tab[i].code == code) {
            str = tab[i].phrase;
            break;
        }
    }
    return str;
}

uint16_t
stun_getw(const uint8_t *ptr)
{
    return ((ptr)[0] << 8) | ptr[1];
}

void *
stun_setw(uint8_t *ptr, uint16_t val)
{
    *ptr++ = val >> 8;
    *ptr++ = val & 0xff;
    return ptr;
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

int
stun_trans_id_to_string(stun_trans_id id, char *buf, size_t len)
{
    if (!buf || len < (3 + 2 * STUN_MSG_TRANS_ID_SIZE)) {
        return -1;
    }
    memset(buf, 0, len);
    int n;
    n = snprintf(buf, len, "0x");
    for (int i = 0; i < STUN_MSG_TRANS_ID_SIZE; i++) {
        n += snprintf(buf + n, len - n, "%02X",
                      id[STUN_MSG_TRANS_ID_SIZE - i - 1]);
    }
    return 0;
}

uint16_t
stun_msg_make_type(stun_msg_class class, stun_msg_type type)
{
    uint16_t r = 0;
    r          = ((class << 4) & 0xFF0) | (type & 0x000F);
    return r;
}

stun_msg_class
stun_msg_get_class(const uint8_t *ptr)
{
    uint16_t t = stun_getw(ptr);
    if (t == 0x0115)
        t = 0x0017;
    return (stun_msg_class)((t & 0x0FF0) >> 4);
}

stun_msg_type
stun_msg_get_type(const uint8_t *ptr)
{
    uint16_t t = stun_getw(ptr);
    if (t == 0x0115)
        t = 0x0017;
    return (stun_msg_type)(((t & 0x3e00) >> 2) | ((t & 0x00e0) >> 1) | (t & 0x000f));
}

uint16_t
stun_msg_length(const uint8_t *ptr)
{
    return stun_getw(ptr +  STUN_MSG_LENGTH_POS) + STUN_MSG_HEADER_SIZE;
}

void
stun_msg_id(const uint8_t *ptr, stun_trans_id id)
{
    memcpy(id, ptr + STUN_MSG_TRANS_ID_POS, STUN_MSG_TRANS_ID_SIZE);
}

uint32_t
stun_msg_magic(const uint8_t *ptr)
{
    union {
        uint16_t tab[2];
        uint32_t magic;
    } m;

    m.tab[0] = ntohs(stun_getw(ptr + STUN_MSG_MAGIC_POS));
    m.tab[1] = ntohs(stun_getw(ptr + STUN_MSG_MAGIC_POS + 2));
    return m.magic;
}

void stun_set_type (uint8_t *h, stun_msg_class c, stun_msg_type m)
{
    uint16_t r = htons(stun_msg_make_type(c,m));
    memcpy(h, &r, 2);
}

int
stun_msg_init(uint8_t *ptr, size_t len, stun_msg_class class,
              stun_msg_type type, const stun_trans_id id)
{
    if (!ptr || len < STUN_MSG_HEADER_SIZE)
        return -1;

    memset(ptr, 0, len);
    stun_set_type(ptr, class, type);
    uint32_t magic = STUN_MAGIC;
    memcpy(ptr + STUN_MSG_MAGIC_POS, &magic, STUN_MSG_MAGIC_SIZE);
    memcpy(ptr + STUN_MSG_TRANS_ID_POS, id, STUN_MSG_TRANS_ID_SIZE);

    return 0;
}


const void *
stun_msg_find(const uint8_t *ptr, stun_attr_type type, uint16_t *palen)
{
    size_t msg_len = stun_msg_length(ptr);
    size_t offset = 0;

    offset = STUN_ATTR_POS;
    while (offset < msg_len) {
        uint16_t atype = stun_getw(ptr + offset);
        size_t alen = stun_getw(ptr + offset + STUN_ATTR_LENGTH_POS);

        offset += STUN_ATTR_VALUE_POS;
        if (atype == type) {
            *palen = alen;
            return ptr + offset;
        }

        switch (atype) {
        case STUN_ATTR_MESSAGE_INTEGRITY:
            if (type == STUN_ATTR_FINGERPRINT)
                break;
            return NULL;
        case STUN_ATTR_FINGERPRINT:
            return NULL;
        default:
            break;
        }
        offset += alen;
    }
    return NULL;
}

int
stun_msg_find_flag(const uint8_t *ptr, stun_attr_type type)
{
    const void *p;
    uint16_t len = 0;
    p = stun_msg_find(ptr, type, &len);
    if (p == NULL) {
        return -1;
    }

    return (len == 0) ? 0 : -2;
}


int
stun_msg_find32(const uint8_t *ptr, stun_attr_type type, uint32_t *pval)
{
    const void *p;
    uint16_t len = 0;

    p = stun_msg_find(ptr, type, &len);
    if (p == NULL) {
        return -1;
    }

    if (len == 4) {
        uint32_t val;
        memcpy(&val, ptr, sizeof(val));
        *pval = ntohl(val);
        return 0;
    }
    return -1;
}

int
stun_msg_find64(const uint8_t *ptr, stun_attr_type type, uint64_t *pval)
{
    const void *p;
    uint16_t len = 0;

    p = stun_msg_find(ptr, type, &len);
    if (p == NULL) {
        return -1;
    }

    if (len == 8) {
        uint32_t tab[2];
        memcpy(tab, ptr, sizeof(tab));
        *pval = ((uint64_t)ntohl(tab[0]) << 32) | ntohl(tab[1]);
        return 0;
    }
    return -1;
}

int
stun_msg_find_string(const uint8_t *ptr, stun_attr_type type, char *buf, size_t buflen)
{
    const unsigned char *p;
    uint16_t alen = 0;

    p = stun_msg_find(ptr, type, &alen);
    if (p == NULL) {
        return -1;
    }

    if (alen >= buflen) {
        return -2;
    }

    memcpy(buf, p, alen);
    buf[alen] = '\0';
    return 0;
}

int
stun_msg_find_addr(const uint8_t *ptr, stun_attr_type type,
                   struct sockaddr_storage *addr, socklen_t *addrlen)
{
    const uint8_t *p;
    uint16_t alen = 0;

    p = stun_msg_find(ptr, type, &alen);
    if (p == NULL) {
        return -1;
    }

    if (alen < 4) {
        return -2;
    }

    switch (p[1]) {
    case 1:
    {
        struct sockaddr_in *ip4 = (struct sockaddr_in *)addr;
        if (((size_t) *addrlen < sizeof(*ip4)) || (alen != 8)) {
            *addrlen = sizeof(*ip4);
            return -2;
        }

        memset(ip4, 0, *addrlen);
        ip4->sin_family = AF_INET;
#ifdef HAVE_SA_LEN
        ip4->sin_len =
#endif
        *addrlen = sizeof(*ip4);
        memcpy(&ip4->sin_port, p + 2, 2);
        memcpy(&ip4->sin_addr, p + 4, 4);
        return 0;
    }
    case 2: {
        struct sockaddr_in6 *ip6 = (struct sockaddr_in6 *)addr;
        if (((size_t) *addrlen < sizeof(*ip6)) || (alen != 20)) {
            *addrlen = sizeof(*ip6);
            return -2;
        }

        memset(ip6, 0, *addrlen);
        ip6->sin6_family = AF_INET6;
#ifdef HAVE_SA_LEN
        ip6->sin6_len =
#endif
        *addrlen = sizeof(*ip6);
        memcpy(&ip6->sin6_port, p + 2, 2);
        memcpy(&ip6->sin6_addr, p + 4, 16);
        return 0;
    }
    default:
        return -3;
    }
}

int
stun_xor_address(const uint8_t *ptr, struct sockaddr_storage *addr, socklen_t addrlen)
{
    union {
        struct sockaddr_storage *addr;
        struct sockaddr_in *in;
        struct sockaddr_in6 *in6;
    } addr_ptr;

    addr_ptr.addr = addr;

    switch (addr->ss_family)
    {
    case AF_INET:
    {
        struct sockaddr_in *ip4 = addr_ptr.in;
        if ((size_t) addrlen < sizeof (*ip4))
            return -1;

        ip4->sin_port ^= htons (STUN_MAGIC >> 16);
        ip4->sin_addr.s_addr ^= htonl (STUN_MAGIC);
        return 0;
    }

    case AF_INET6:
    {
        struct sockaddr_in6 *ip6 = addr_ptr.in6;
        unsigned short i;

        if ((size_t) addrlen < sizeof (*ip6))
            return -1;

        ip6->sin6_port ^= htons (STUN_MAGIC >> 16);
        for (i = 0; i < 16; i++)
            ip6->sin6_addr.s6_addr[i] ^= ptr[STUN_MSG_MAGIC_POS + i];
        return 0;
    }

    default:
        return -1;
    }
}
int
stun_msg_find_xor_addr(const uint8_t *ptr, stun_attr_type type,
                       struct sockaddr_storage *addr, socklen_t *addrlen)
{
    int rc = stun_msg_find_addr(ptr, type, addr, addrlen);
    if (rc != 0) {
        return rc;
    }
    return stun_xor_address(ptr, addr, *addrlen);
}

int
stun_msg_find_error(const uint8_t *ptr, int *code)
{
    uint16_t alen = 0;
    const uint8_t *p = stun_msg_find(ptr, STUN_ATTR_ERROR_CODE, &alen);
    uint8_t class, number;

    if (p == NULL) {
        return -1;
    }
    if (alen < 4) {
        return -2;
    }

    class = p[2] & 0x7;
    number = p[3];

    if ((class < 3) || (class > 6) || (number > 99))
        return -1;

    *code = (class * 100) + number;
    return 0;
}

int
stun_msg_find_error_string(const uint8_t *ptr, int *code, char *buf, size_t buf_len)
{
    uint16_t alen = 0;
    const uint8_t *p = stun_msg_find(ptr, STUN_ATTR_ERROR_CODE, &alen);
    uint8_t class, number;

    if (p == NULL) {
        return -1;
    }
    if (alen < 4) {
        return -2;
    }

    class = p[2] & 0x7;
    number = p[3];

    if ((class < 3) || (class > 6) || (number > 99))
        return -1;

    *code = (class * 100) + number;

    if (alen - 4 > buf_len) {
        return -3;
    }
    memcpy(buf, p + 4, alen - 4);
    buf[alen-4] = '\0';
    return 0;
}

uint8_t *
stun_msg_append(uint8_t *ptr, size_t len, stun_attr_type type, size_t data_len)
{
    uint16_t mlen = stun_msg_length(ptr);
    uint8_t *a;
    if (mlen + STUN_ATTR_HEADER_SIZE + data_len > len) {
        return NULL;
    }

    a = ptr + mlen;

    a = stun_setw(a, type);
    a = stun_setw(a, data_len);

    mlen += (STUN_ATTR_HEADER_SIZE + data_len);
    stun_setw(ptr + STUN_MSG_LENGTH_POS, mlen - STUN_MSG_HEADER_SIZE);
    return a;
}

int
stun_msg_append_bytes(uint8_t *ptr, size_t len, stun_attr_type type, const void *data, size_t data_len)
{
    void *p = stun_msg_append(ptr, len, type, data_len);
    if (p == NULL) {
        return -1;
    }
    if (data_len > 0)
        memcpy(p, data, data_len);
    return 0;
}

int
stun_msg_append_flag(uint8_t *ptr, size_t len, stun_attr_type type)
{
    return stun_msg_append_bytes(ptr, len, type, NULL, 0);
}

int
stun_msg_append32(uint8_t *ptr, size_t len, stun_attr_type type, uint32_t value)
{
    value = htonl(value);
    return stun_msg_append_bytes(ptr, len, type, &value, 4);
}

int
stun_msg_append64(uint8_t *ptr, size_t len, stun_attr_type type, uint64_t value)
{
    uint32_t tab[2];
    tab[0] = htonl((uint32_t) (value >> 32));
    tab[1] = htonl((uint32_t) value);

    return stun_msg_append_bytes(ptr, len, type, tab, 8);
}

int
stun_msg_append_string(uint8_t *ptr, size_t len, stun_attr_type type, const char *str)
{
    return stun_msg_append_bytes(ptr, len, type, str, strlen(str));
}

static const char utf8_skip_data[256] = {
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,
    3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,4,4,4,4,4,4,4,4,5,5,5,5,6,6,1,1
};

#define next_utf8_char(p) (char *)((p) +                                \
                                   utf8_skip_data[*(const unsigned char *)(p)])
#define PACKAGE_STRING "stun"
int
stun_msg_append_software(uint8_t *ptr, size_t len, const char *software)
{
    int alen = 0;
    const char *p = NULL;
    if (software == NULL) {
        software = PACKAGE_STRING;
    }
    p = software;
    while (*p && alen < 128) {
        p = next_utf8_char(p);
        alen++;
    }
    return stun_msg_append_bytes(ptr, len, STUN_ATTR_SOFTWARE, software, p - software);
}

int
stun_msg_append_addr(uint8_t *ptr, size_t len, stun_attr_type type,
                     const struct sockaddr *addr, socklen_t addrlen)
{
    uint8_t *p;
    const void *paddr;
    uint16_t alen, port;
    uint8_t family;
    struct sockaddr_storage ss;

    union {
        const struct sockaddr *addr;
        const struct sockaddr_in *in;
        const struct sockaddr_in6 *in6;
    } sa;

    if (addrlen < sizeof(struct sockaddr))
        return -1;

    sa.addr = addr;
    switch (addr->sa_family) {
    case AF_INET:
    {
        memcpy(&ss, sa.in, sizeof(struct sockaddr_in));
        struct sockaddr_in *ip4 = (struct sockaddr_in *)&ss;
        family = 1;
        if (type == STUN_ATTR_XOR_MAPPED_ADDRESS) {
            ip4->sin_port ^= htons(STUN_MAGIC >> 16);
            ip4->sin_addr.s_addr ^= htonl(STUN_MAGIC);
        }
        port = ip4->sin_port;
        paddr = &ip4->sin_addr;
        alen = 4;
        break;
    }
    case AF_INET6:
    {
        memcpy(&ss, sa.in6, sizeof(struct sockaddr_in6));
        struct sockaddr_in6 *ip6 = (struct sockaddr_in6 *)&ss;
        if (addrlen < sizeof(*ip6))
            return -1;
        family = 2;
        if (type == STUN_ATTR_XOR_MAPPED_ADDRESS) {
            ip6->sin6_port ^= htons(STUN_MAGIC >> 16);
            for (int i = 0; i < 16; i++) {
                ip6->sin6_addr.s6_addr[i] ^= ptr[STUN_MSG_MAGIC_POS + i];
            }
        }
        port = ip6->sin6_port;
        alen = 16;
        paddr = &ip6->sin6_addr;
        break;
    }
    default:
        return -2;

    }

    p = stun_msg_append(ptr, len, type, 4 + alen);
    if (!p) {
        return -3;
    }
    p[0] = 0;
    p[1] = family;
    memcpy(p + 2, &port, 2);
    memcpy(p + 4, paddr, alen);
    return 0;
}

int
stun_msg_append_xor_addr(uint8_t *ptr, size_t len, stun_attr_type type,
                         const struct sockaddr_storage *addr, socklen_t addrlen)
{
    int rc;
    struct sockaddr_storage tmpaddr;

    if ((size_t)addrlen > sizeof(tmpaddr)) {
        addrlen = sizeof(tmpaddr);
    }
    memcpy(&tmpaddr, addr, addrlen);

    rc = stun_xor_address(ptr, &tmpaddr, addrlen);
    if (rc) {
        return rc;
    }

    return stun_msg_append_addr(ptr, len, type, (struct sockaddr *)&tmpaddr, addrlen);
}

int
stun_msg_append_error(uint8_t *ptr, size_t len, stun_error code)
{
    const char *str = stun_strerror(code);
    size_t alen = strlen(str);

    uint8_t *p = stun_msg_append(ptr, len, STUN_ATTR_ERROR_CODE, 4 + alen);
    if (p == NULL) {
        return -1;
    }

    memset(p, 0, 2);
    p[2] = code / 100;
    p[3] = code % 100;
    memcpy(p +  4, str, len);
    return 0;
}


ssize_t
stun_get_binding_request(uint8_t *ptr, size_t len, stun_trans_id id)
{

    ssize_t alen = 0;
    /* int rc = stun_msg_init(ptr, len, STUN_CLASS_REQUEST, STUN_ALLOCATE, id); */
    int rc = stun_msg_init(ptr, len, STUN_CLASS_REQUEST, STUN_BINDING, id);
    if (rc != 0) {
        return rc;
    }
    alen += STUN_MSG_HEADER_SIZE;

    /* rc = stun_msg_append32(ptr, len, STUN_ATTR_LIFETIME, 3600);
     * if (rc != 0) {
     *     return rc;
     * }
     * alen += (STUN_ATTR_HEADER_SIZE + 4); */

    /* rc = stun_msg_append32(ptr, len, STUN_ATTR_REQUESTED_TRANSPORT, 12345);
     * if (rc != 0) {
     *     return rc;
     * }
     * alen += (STUN_ATTR_HEADER_SIZE + 4); */

    /* rc = stun_msg_append_flag(ptr, len, STUN_ATTR_DONT_FRAGMENT);
     * if (rc != 0) {
     *     return rc;
     * }
     * alen += STUN_ATTR_HEADER_SIZE; */

    rc = stun_msg_append_software(ptr, len, "abcd");
    if (rc != 0) {
        return rc;
    }
    alen += (STUN_ATTR_HEADER_SIZE + 4);

    return alen;
}

ssize_t
stun_get_binding_response(uint8_t *ptr, size_t len, stun_trans_id id,
                          struct sockaddr *addr, socklen_t addrlen)
{
    if (!ptr || !addr) {
        return -1;
    }

    int rc;
    rc = stun_msg_init(ptr, len, STUN_CLASS_RESPONSE, STUN_BINDING, id);
    if (rc) {
        return rc;
    }

    rc = stun_msg_append_addr(ptr, len, STUN_ATTR_MAPPED_ADDRESS, addr, addrlen);
    if (rc) {
        return rc;
    }

    rc = stun_msg_append_addr(ptr, len, STUN_ATTR_XOR_MAPPED_ADDRESS, addr, addrlen);
    if (rc) {
        return rc;
    }

    rc = stun_msg_append_software(ptr, len, "abcdefg");
    if (rc) {
        return rc;
    }
    return stun_msg_length(ptr);
}

int
stun_parse_response(uint8_t *ptr, size_t len)
{
    if (!ptr || len <= 0) {
        Error("invalid argument");
        return -1;
    }

    stun_trans_id id;
    char idstr[32];
    Debug("type: %s", stun_msg_type_to_string((stun_msg_get_class(ptr) << 4 | stun_msg_get_type(ptr))));
    Debug("length: %u", stun_msg_length(ptr));
    Debug("magic: 0x%X", stun_msg_magic(ptr));
    stun_msg_id(ptr,id);
    stun_trans_id_to_string(id,idstr,sizeof(idstr));
    Debug("id: %s", idstr);

    uint16_t attr_len;
    const void *pattr;

    /* STUN_ATTR_MAPPED_ADDRESS 0x0001 */
    pattr = stun_msg_find(ptr, STUN_ATTR_MAPPED_ADDRESS, &attr_len);
    if (pattr) {
        Debug("MAPPED_ADDRESS: %u", attr_len);
        struct sockaddr_storage addr;
        socklen_t addrlen = sizeof(addr);
        char ip[64];
        if (stun_msg_find_addr(ptr, STUN_ATTR_MAPPED_ADDRESS, &addr, &addrlen) == 0) {
            switch (addr.ss_family) {
            case AF_INET: {
                struct sockaddr_in *ip4 = (struct sockaddr_in *)&addr;
                inet_ntop(AF_INET, &ip4->sin_addr, ip, sizeof(ip));
                Debug("ip: %s:%d", ip, htons(ip4->sin_port));
                break;
            }
            case AF_INET6: {
                struct sockaddr_in6 *ip6 = (struct sockaddr_in6 *)&addr;
                inet_ntop(AF_INET6, &ip6->sin6_addr, ip, sizeof(ip));
                Debug("ip: %s:%d", ip, htons(ip6->sin6_port));
                break;
            }
            default:
                break;
            }
        }
    }

    /* STUN_ATTR_RESPONSE_ADDRESS 0x0002 */
    pattr = stun_msg_find(ptr, STUN_ATTR_RESPONSE_ADDRESS, &attr_len);
    if (pattr) {
        Debug("RESPONSE_ADDRESS: %u", attr_len);
        struct sockaddr_storage addr;
        socklen_t addrlen = sizeof(addr);
        char ip[64];
        if (stun_msg_find_addr(ptr, STUN_ATTR_MAPPED_ADDRESS, &addr, &addrlen) == 0) {
            switch (addr.ss_family) {
            case AF_INET: {
                struct sockaddr_in *ip4 = (struct sockaddr_in *)&addr;
                inet_ntop(AF_INET, &ip4->sin_addr, ip, sizeof(ip));
                Debug("ip: %s:%d", ip, htons(ip4->sin_port));
                break;
            }
            case AF_INET6: {
                struct sockaddr_in6 *ip6 = (struct sockaddr_in6 *)&addr;
                inet_ntop(AF_INET6, &ip6->sin6_addr, ip, sizeof(ip));
                Debug("ip: %s:%d", ip, htons(ip6->sin6_port));
                break;
            }
            default:
                break;
            }
        }
    }

    /* STUN_ATTR_CHANGE_REQUEST 0x0003 */
    pattr = stun_msg_find(ptr, STUN_ATTR_CHANGE_REQUEST, &attr_len);
    if (pattr) {
        Debug("CHANGE_REQUEST: %u", attr_len);
    }

    /* STUN_ATTR_SOURCE_ADDRESS 0x0004 */
    pattr = stun_msg_find(ptr, STUN_ATTR_SOURCE_ADDRESS, &attr_len);
    if (pattr) {
        Debug("SOURCE_ADDRESS: %u", attr_len);
        struct sockaddr_storage addr;
        socklen_t addrlen = sizeof(addr);
        char ip[64];
        if (stun_msg_find_addr(ptr, STUN_ATTR_MAPPED_ADDRESS, &addr, &addrlen) == 0) {
            switch (addr.ss_family) {
            case AF_INET: {
                struct sockaddr_in *ip4 = (struct sockaddr_in *)&addr;
                inet_ntop(AF_INET, &ip4->sin_addr, ip, sizeof(ip));
                Debug("ip: %s:%d", ip, htons(ip4->sin_port));
                break;
            }
            case AF_INET6: {
                struct sockaddr_in6 *ip6 = (struct sockaddr_in6 *)&addr;
                inet_ntop(AF_INET6, &ip6->sin6_addr, ip, sizeof(ip));
                Debug("ip: %s:%d", ip, htons(ip6->sin6_port));
                break;
            }
            default:
                break;
            }
        }
    }

    /* STUN_ATTR_CHANGED_ADDRESS 0x0005 */
    pattr = stun_msg_find(ptr, STUN_ATTR_CHANGED_ADDRESS, &attr_len);
    if (pattr) {
        Debug("CHANGED_ADDRESS: %u", attr_len);
        struct sockaddr_storage addr;
        socklen_t addrlen = sizeof(addr);
        char ip[64];
        if (stun_msg_find_addr(ptr, STUN_ATTR_MAPPED_ADDRESS, &addr, &addrlen) == 0) {
            switch (addr.ss_family) {
            case AF_INET: {
                struct sockaddr_in *ip4 = (struct sockaddr_in *)&addr;
                inet_ntop(AF_INET, &ip4->sin_addr, ip, sizeof(ip));
                Debug("ip: %s:%d", ip, htons(ip4->sin_port));
                break;
            }
            case AF_INET6: {
                struct sockaddr_in6 *ip6 = (struct sockaddr_in6 *)&addr;
                inet_ntop(AF_INET6, &ip6->sin6_addr, ip, sizeof(ip));
                Debug("ip: %s:%d", ip, htons(ip6->sin6_port));
                break;
            }
            default:
                break;
            }
        }
    }

    /* STUN_ATTR_USERNAME  0x0006 */
    pattr = stun_msg_find(ptr, STUN_ATTR_USERNAME, &attr_len);
    if (pattr) {
        Debug("USERNAME :%u", attr_len);
        char username[128];
        if (stun_msg_find_string(ptr, STUN_ATTR_USERNAME, username, sizeof(username)) == 0) {
            Debug("username: %s", username);
        }
    }

    /* STUN_ATTR_PASSWORD 0x0007 */
    pattr = stun_msg_find(ptr, STUN_ATTR_PASSWORD, &attr_len);
    if (pattr) {
        Debug("PASSWORD: %u", attr_len);
        char passwd[128];
        if (stun_msg_find_string(ptr, STUN_ATTR_PASSWORD, passwd, sizeof(passwd)) == 0) {
            Debug("password: %s", passwd);
        }
    }

    /* STUN_ATTR_MESSAGE_INTEGRITAY 0x0008 */
    pattr = stun_msg_find(ptr, STUN_ATTR_MESSAGE_INTEGRITY, &attr_len);
    if (pattr) {
        Debug("MESSAGE_INTEGRITY: %u", attr_len);

    }

    /* STUN_ATTR_ERROR_CODE 0x0009 */
    int error;
    if (stun_msg_find_error(ptr,&error) == 0) {
        Error("ERROR_CODE: %d", error);
        char err_msg[128];
        if (stun_msg_find_error_string(ptr, &error, err_msg, sizeof(err_msg)) == 0) {
            Error("Error: %s", err_msg);
        }
    }

    /* STUN_ATTR_UNKNOWN_ATTRIBUTES 0x000A */
    pattr = stun_msg_find(ptr, STUN_ATTR_UNKNOWN_ATTRIBUTES, &attr_len);
    if (pattr) {
        Debug("UNKNOWN_ATTRIBUTES: %u", attr_len);
        uint16_t ua;
        uint16_t alen = 0;
        while (alen < attr_len) {
            ua = stun_getw(pattr + alen);
            Debug("unknown attr: 0x%04X", ua);
            alen += 4;
        }
    }

    /* STUN_ATTR_XOR_MAPPED_ADDRESS 0x0020 */
    pattr = stun_msg_find(ptr, STUN_ATTR_XOR_MAPPED_ADDRESS, &attr_len);
    if (pattr) {
        Debug("XOR_MAPPED_ADDRESS: %u", attr_len);
        struct sockaddr_storage addr;
        socklen_t addrlen = sizeof(addr);
        char ip[64];
        if (stun_msg_find_xor_addr(ptr, STUN_ATTR_XOR_MAPPED_ADDRESS, &addr, &addrlen) == 0) {
            switch (addr.ss_family) {
            case AF_INET: {
                struct sockaddr_in *ip4 = (struct sockaddr_in *)&addr;
                inet_ntop(AF_INET, &ip4->sin_addr, ip, sizeof(ip));
                Debug("ip: %s:%d", ip, htons(ip4->sin_port));
                break;
            }
            case AF_INET6: {
                struct sockaddr_in6 *ip6 = (struct sockaddr_in6 *)&addr;
                inet_ntop(AF_INET6, &ip6->sin6_addr, ip, sizeof(ip));
                Debug("ip: %s:%d", ip, htons(ip6->sin6_port));
                break;
            }
            default:
                break;
            }
        }
    }

    /* STUN_ATTR_SOFTWARE 0x8022 */
    pattr = stun_msg_find(ptr, STUN_ATTR_SOFTWARE, &attr_len);
    if (pattr) {
        Debug("SOFTWARE: %u", attr_len);
        char software[128];
        if (stun_msg_find_string(ptr, STUN_ATTR_SOFTWARE, software, sizeof(software)) == 0) {
            Debug("software: %s", software);
        }
    }

    return 0;
}

void
stun_msg_dump(const uint8_t *ptr)
{
    size_t msg_len = stun_msg_length(ptr);
    size_t offset = 0;

    offset = STUN_ATTR_POS;
    while (offset < msg_len) {
        uint16_t atype = stun_getw(ptr + offset);
        size_t alen = stun_getw(ptr + offset + STUN_ATTR_LENGTH_POS);

        offset += STUN_ATTR_VALUE_POS;
        Debug("attr: 0x%03X %lu", atype, alen);
        offset += alen;
    }
}
