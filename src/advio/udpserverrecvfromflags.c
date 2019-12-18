/*
 * recvfrom_flags.c - recvfrom_flags
 *
 * Date   : 2019/12/18
 */
#include "unp.h"

int
main(int argc, char *argv[])
{
    int sockfd;
    struct sockaddr_in servaddr, cliaddr;

    sockfd = Socket(AF_INET, SOCK_DGRAM, 0);

    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family      = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port        = htons(SERV_PORT);

    Bind(sockfd, (SA *)&servaddr, sizeof(servaddr));

    dg_echo_recvfromflags(sockfd, (SA *)&cliaddr, sizeof(cliaddr));

    return 0;
}
