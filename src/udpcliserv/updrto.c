/*
 * updrto.c - updrto
 *
 * Date   : 2019/12/21
 */

/*
 * First Time:
 * SRTT = RTT
 * RTTVAR = RTT/2
 * RTO = SRTT + min(G, K*RTTVAR)
 *
 * Other:
 * RTTVAR = (1-beta) * SRTT + beta * |SRTT-RTT|
 * SRTT= (1-alpha) * SRTT + alpha * RTT
 * RTO = SRTT + MAX(G, K*RTTVAR)
 *
 * K=4
 * alpha=1/8
 * beta=1/4 */
