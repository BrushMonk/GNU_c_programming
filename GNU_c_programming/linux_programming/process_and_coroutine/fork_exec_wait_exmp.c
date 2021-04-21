#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/resource.h>
int global_var=1;
int main(void)
{
    static int static_var = 2;
    puts("This is parent process.")
    /* RLIM_INFINITY means an infinite limit in the process resources.
    RLIMIT_NPROC is the maximum number of child processes per real user ID.
    Changing this limit affects the value returned for _SC_CHILD_MAX by the sysconf function */
    if (setrlimit(RLIMIT_NPROC, &(struct rlimit){RLIM_INFINITY >> 5, RLIM_INFINITY >> 5}) < 0)
    {
        perror("setrlimit RLIMIT_NPROC error");
        exit(EXIT_FAILURE);
    }
    /* RLIMIT_AS is the maximum size in bytes of a process's total available
    memory. This affects the sbrk function and the mmap function. */
    if (setrlimit(RLIMIT_AS, &(struct rlimit){32768, 32768}) < 0) /* 32768 bytes */
    {
        perror("setrlimit RLIMIT_AS error");
        exit(EXIT_FAILURE);
    }
    /* RLIMIT_STACK is the maximum size in bytes of the preocess's stack */
    if (setrlimit(RLIMIT_STACK, &(struct rlimit){4096, 4096}) < 0) /* 4096 bytes */
    {
        perror("setrlimit RLIMIT_STACK error");
        exit(EXIT_FAILURE);
    }
    pid_t child_pid[RLIMIT_NPROC];
    int child_status[RLIMIT_NPROC];
    for (__rlim_t i = 0; i < RLIMIT_NPROC; i++)
    {
        child_pid[i] = fork();
        if (i = 0 && child_pid[i] == 0)
        {
            /* When a process calls one of the exec functions, that process is
            completely replaced by the new program, and the new program starts
            executing at its main function. */
            if (execl("/home/brushmonk/brushmonk_repository/GNU_c_programming/"
            "parse_program_argument/getopt_example.exe" ,"92 -a84 -ccc a") == -1)
            perror("execl error");
        }
        else if (child_pid[i] == 0)
        {
            /* global variable and static variable are copied into child process
            and unrelated to those ones in parent process */
            ++global_var, ++static_var;
            break;
        }
        else
        {
            /* waipid(-1, &status, 0) equals wait(&status) */
            if ( waitpid(child_pid[i], &child_status[i], WNOHANG | WUNTRACED) < 0 )
                perror("waitpid error");
            if ( WIFEXITED(child_status[i]) )
			    printf("child_pid %d exited with code%d\n", child_pid[i], WEXITSTATUS(child_status[i]));
            else if ( WIFSIGNALED(child_status[i]) )
			printf("child_pid %d terminated abnormally, signal %d\n", child_pid[i], WTERMSIG(child_status[i]));
        }
    }
    printf("pid %d is a ", getpid());
    if (child_pid[i] == 0) printf("child");
    else printf("father");
    printf(" process, global_var == %d, static_var == %d\n",  ,global_var, static_var);
    exit(EXIT_SUCCESS);
}