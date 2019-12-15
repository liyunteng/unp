/*
 * checkudpsum.c - checkudpsum
 *
 * Date   : 2019/12/15
 */
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/syscall.h>
#include <linux/sysctl.h>

int _sysctl(struct __sysctl_args *args);

#define OSNAMESZ 128
int main(int argc, char *argv[])
{
    struct __sysctl_args args;
    char osname[OSNAMESZ];
    size_t osnamelen;
    int name[] = {CTL_KERN, KERN_OSTYPE};

    memset(&args, 0, sizeof(struct __sysctl_args));
    args.name = name;
    args.nlen = sizeof(name)/sizeof(name[0]);
    args.oldval = osname;
    args.oldlenp = &osnamelen;
    osnamelen = sizeof(osname);

    if (syscall(SYS__sysctl, &args) == -1) {
        perror("_sysctl");
        exit(EXIT_FAILURE);
    }
    printf("This machine is running %s\n", osname);

    return 0;
}
