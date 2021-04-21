#include <stdio.h>
#include <stdlib.h>
#include <setjmp.h>
static jmp_buf env_a, env_b;
void routine_a(void);
void routine_b(void);

void routine_a(void)
{
    int a;

    printf("(a1)\n"); /* 1st step */

    a=setjmp(env_a);
    if(a==0)routine_b(); /* goes to 2nd step */

    printf("(a2) a == %d\n", a); /* 3rd step */

    a=setjmp(env_a);
    if(a==0)longjmp(env_b, 20001); /* set b=20001, then goes to 4th step */

    printf("(a3) a == %d\n", a); /* 5th step */

    a=setjmp(env_a);
    if(a==0)longjmp(env_b, 20002); /* set b=20002, then goes to 6th step */

    printf("(a4) a == %d\n", a); /* 7th step */
}

void routine_b(void)
{
    int b;

    printf("(b1)\n"); /* 2nd step */

    b=setjmp(env_b);
    if(b==0)longjmp(env_a, 10001); /* set a=10001, then goes to 3rd step */

    printf("(b2) b == %d\n", b); /* 4th step */

    b=setjmp(env_b);
    if(b==0)longjmp(env_a, 10002); /* set a=10002, then goes to 5th step */

    printf("(B3) b == %d\n", b); /* 6th step */

    b=setjmp(env_b);
    if(b==0)longjmp(env_a, 10003); /* set a=10003, then goes to 7th step */
}

int main(int argc, char *argv[])
{
    routine_a(); /* starting point */
    exit(EXIT_SUCCESS);}