/*
 * dg_send_recv.c - dg_send_recv
 *
 * Date   : 2019/12/23
 */
#include "unprtt.h"
#include <setjmp.h>

#define RTT_DEBUG

static struct rtt_info rttinfo;
static int rttinit = 0;
static struct msghdr msgsend, msgrecv;
static struct hdr {
    uint32_t seq;
    uint32_t ts;
} sendhdr, recvhdr;

static void sig_alarm(int signo);
static sigjmp_buf jmpbuf;

ssize_t
dg_send_recv(int fd, const void *outbuf, size_t outbytes,
             void *inbuf, size_t inbytes,
             const SA *destaddr, socklen_t destlen)
{
    ssize_t n;
    struct iovec iovsend[2], iovrecv[2];

    if (rttinit == 0) {
        rtt_init(&rttinfo);
        rttinit = 1;
        rtt_d_flag = 1;
    }

    sendhdr.seq++;
    msgsend.msg_name = (void *)destaddr;
    msgsend.msg_namelen = destlen;
    msgsend.msg_iov = iovsend;
    msgsend.msg_iovlen = 2;
    iovsend[0].iov_base = &sendhdr;
    iovsend[0].iov_len = sizeof(struct hdr);
    iovsend[1].iov_base = (void *)outbuf;
    iovsend[1].iov_len = outbytes;

    msgrecv.msg_name = NULL;
    msgrecv.msg_namelen = 0;
    msgrecv.msg_iov = iovrecv;
    msgrecv.msg_iovlen = 2;
    iovrecv[0].iov_base = &recvhdr;
    iovrecv[0].iov_len = sizeof(struct hdr);
    iovrecv[1].iov_base = inbuf;
    iovrecv[1].iov_len = inbytes;

    Signal(SIGALRM, sig_alarm);
    rtt_newpack(&rttinfo);

sendagain:
    sendhdr.ts = rtt_ts(&rttinfo);
    sendmsg(fd, &msgsend, 0);

    alarm(rtt_start(&rttinfo));
    if (sigsetjmp(jmpbuf, 1) != 0) {
        if (rtt_timeout(&rttinfo) < 0) {
            err_msg("dg_send_recv: no response from server, giving up");
            rttinit = 0;
            errno = ETIMEDOUT;
            return(-1);
        }
        goto sendagain;
    }

    do {
        n = recvmsg(fd, &msgrecv, 0);
    } while (n < (ssize_t)sizeof(struct hdr) || recvhdr.seq != sendhdr.seq);

    alarm(0);
    rtt_stop(&rttinfo, rtt_ts(&rttinfo) - recvhdr.ts);

    return (n - sizeof(struct hdr));
}

static void
sig_alarm(int signo)
{
    siglongjmp(jmpbuf, 1);
}

void
dg_cli_rtt(FILE *fp, int sockfd, const SA *pservaddr, socklen_t servlen)
{
    ssize_t n;
    char sendline[MAXLINE], recvline[MAXLINE+1];

    while (Fgets(sendline, MAXLINE, fp) != NULL) {
        n = dg_send_recv(sockfd, sendline, strlen(sendline),
                         recvline, MAXLINE, pservaddr, servlen);
        if (n > 0) {
            recvline[n] = 0;
            Fputs(recvline, stdout);
        }
    }
}

int
main(int argc, char *argv[])
{
    int sockfd;
    struct sockaddr_in servaddr;

    if (argc != 2) {
        err_quit("Usage: %s <IPaddress>", argv[0]);
    }

    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port   = htons(SERV_PORT);
    Inet_pton(AF_INET, argv[1], &servaddr.sin_addr);

    sockfd = Socket(AF_INET, SOCK_DGRAM, 0);

    dg_cli_rtt(stdin, sockfd, (SA *)&servaddr, sizeof(servaddr));

    return 0;
}
