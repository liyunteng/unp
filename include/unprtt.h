 /*
 * unprtt.h - unprtt
 *
 * Date   : 2019/12/23
 */
#ifndef UNPRTT_H
#define UNPRTT_H

#include "unp.h"
/*
 * First Time:
 * SRTT = RTT
 * RTTVAR = RTT/2
 * RTO = SRTT + min(G, K*RTTVAR)
 *
 * Other:
 * RTTVAR = (1-beta) * RTTVAR + beta * |SRTT-RTT|
 * SRTT= (1-alpha) * SRTT + alpha * RTT
 * RTO = SRTT + MAX(G, K*RTTVAR)
 *
 * K=4
 * alpha=1/8
 * beta=1/4 */

struct rtt_info {
    float rtt_rtt;              /* most recent measured RTT, in seconds */
    float rtt_srtt;             /* smoothed RTT estimator, in seconds */
    float rtt_rttvar;           /* smoothed mean deviation, in seconds */
    float rtt_rto;              /* current RTO to use, in seconds */
    int rtt_nrexmt;             /* # times retransimitted; 0, 1, 2, ... */
    uint32_t rtt_base;          /* # sec since 1/1/1970 at start */
};

#define RTT_RXTMIN 1            /* min retransmit timeout value, in seconds */
#define RTT_RXTMAX 60           /* max retransmit timeout value, in seconds */
#define RTT_MAXNREXMT 3         /* max # times to retransmit */

void rtt_debug(struct rtt_info *);
void rtt_init(struct rtt_info*);
void rtt_newpack(struct rtt_info*);
int rtt_start(struct rtt_info*);
void rtt_stop(struct rtt_info*, uint32_t);
int rtt_timeout(struct rtt_info*);
uint32_t rtt_ts(struct rtt_info*);

extern int rtt_d_flag;

#endif
