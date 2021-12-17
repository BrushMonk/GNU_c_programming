#include <unistd.h>
#include <sys/resource.h>
#include <sys/types.h>
#include <stdlib.h>
#include <stdio.h>
static void process_limits(int resource, char *resc_name)
{
    struct rlimit cur_procs_limit;
    unsigned long long lim;
    if(getrlimit(resource, &cur_procs_limit)<0)
        fprintf(stderr, "getrlimit error for %s", resc_name);
        printf("%-14s ", resc_name);
    if(cur_procs_limit.rlim_cur==RLIM_INFINITY)
        printf("(infinite) ");
    else
    {
        lim=cur_procs_limit.rlim_cur;
        printf("%10lld ", lim);
    }
    if(cur_procs_limit.rlim_max==RLIM_INFINITY)
        printf("(infinite)");
    else
    {
        lim=cur_procs_limit.rlim_max;
        printf("%10lld", lim);
    }
    putchar('\n');
}
#define process_limits(resource) process_limits(resource, #resource)

int main(void)
{
    #ifdef RLIMIT_AS
    process_limits(RLIMIT_AS);
    #endif
    process_limits(RLIMIT_CORE);
    process_limits(RLIMIT_CPU);
    process_limits(RLIMIT_DATA);
    process_limits(RLIMIT_FSIZE);
    #ifdef RLIMIT_MEMLOCK
    process_limits(RLIMIT_MEMLOCK);
    #endif
    #ifdef RLIMIT_MSGQUEUE
    process_limits(RLIMIT_MSGQUEUE);
    #endif
    #ifdef RLIMIT_NICE
    process_limits(RLIMIT_NICE);
    #endif
    process_limits(RLIMIT_NOFILE);
    #ifdef RLIMIT_NPROC
    process_limits(RLIMIT_NPROC);
    #endif
    #ifdef RLIMIT_NPTS
    process_limits(RLIMIT_NPTS);
    #endif
    #ifdef RLIMIT_RSS
    process_limits(RLIMIT_RSS);
    #endif
    #ifdef RLIMIT_SBSIZE
    process_limits(RLIMIT_SBSIZE);
    #endif
    #ifdef RLIMIT_SIGPENDING
    process_limits(RLIMIT_SIGPENDING);
    #endif
    process_limits(RLIMIT_STACK);
    #ifdef RLIMIT_SWAP
    process_limits(RLIMIT_SWAP);
    #endif
    #ifdef RLIMIT_VMEM
    process_limits(RLIMIT_VMEM);
    #endif
    exit(EXIT_SUCCESS);
}