/*
 * tcpserv04.c - tcpserv04
 *
 * Date   : 2019/11/10
 */
#include "unp.h"

void
sig_chld(int signo)
{
    pid_t pid;
    int stat;

    while ((pid = waitpid(-1, &stat, WNOHANG)) > 0) {
        printf("child %d terminated\n", pid);
    }
    return;
}

int main(int argc, char *argv[])
{
    int listenfd, connfd;
    pid_t childpid;
    socklen_t clilen;
    struct sockaddr_in cliaddr, servaddr;

    listenfd = Socket(AF_INET, SOCK_STREAM, 0);

    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(SERV_PORT);
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);

    Bind(listenfd, (SA*)&servaddr, sizeof(servaddr));
    Listen(listenfd, LISTENQ);
    Signal(SIGCHLD, sig_chld);

    for(;;) {
        clilen = sizeof(cliaddr);
        if ((connfd = accept(listenfd, (SA*)&cliaddr, &clilen)) < 0) {
            if (errno == EINTR)
                continue;
            else
                err_sys("accept error");
        }

        if ((childpid = fork()) == 0) {
            Close(listenfd);
            str_echo(connfd);
            exit(0);
        }
        Close(connfd);
    }


    return 0;
}
