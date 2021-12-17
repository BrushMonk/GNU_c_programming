#include <signal.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
int main(int argc, char *argv[])
{
    int s, sig;
    if(argc!=3 || strcmp(argv[1], "--help")==0)
        perror("%s sig-num pid\n", argv[0]);
    sig=atoi(argv[2]);
    s=kill(atol(argv[1]), sig);
    if(sig!=0) {
        if(s==-1) {
            perror("kill");
            exit(EXIT_FAILURE);
        }
    }
    else { /* Null signal: process existence check */
        if(s==0) puts("Process exists and we can send it a signal");
        else {
            if(errno==EPERM) puts("Process exists, but we don't have permission to send it a signal");
            else if(errno==ESRCH) puts("Process does not exist");
            else {
                perror("kill");
                exit(EXIT_FAILURE);
            }
        }
    }
    exit(EXIT_SUCCESS);
}