#include <math.h>
#include <stdio.h>
#include <float.h>
#include <limits.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdbool.h>
#define charjudge(c) \
printf("Is \'%c\' alphabetic or numeric?: %d\n", c, isalnum(c));\
printf("Is \'%c\' alphabetic?: %d\n", c, isalpha(c));\
printf("Is \'%c\' a space, horizontal tab, or newline: %d\n", c, isblank(c));\
printf("Is \'%c\' a control character: %d\n", c, iscntrl(c));\
printf("Is \'%c\' a digit: %d\n", c, isdigit(c));\
printf("Is \'%c\' a printing character: %d\n", c, isprint(c));\
printf("Is \'%c\' a punctuation character: %d\n", c, ispunct(c));\
printf("Is \'%c\' a space, newline, formfeed, carriage return, vertical tab, horizontal tab: %d\n", c, isspace(c));\
printf("Is \'%c\' an uppercase character: %d", c, isupper(c))

int main(void)
{
    /* <math.h> library */
    printf("isinf(INFINITY) == %d\n", isinf(INFINITY));
    /* macro NAN means not a number */
    printf("isnan(NAN) == %d\n", isnan(NAN));
    printf("isnan(1/0) == %d\n", isnan(1.0/0.0));
    printf("isnormal(INFINITY) == %d\n", isnormal(INFINITY));
    printf("isnormal(NAN) == %d\n", isnormal(NAN));
    /* DBL_MIN/19.0 is a subnormal floating number, which is not normal. */
    printf("isnormal(DBL_MIN/19.0) == %d\n", isnormal(DBL_MIN/19.0));
    printf("isnormal(0.0) == %d\n", isnormal(0.0));
    printf("The absolute value of LONG_MIN equals %lld\n", llabs(LLONG_MIN));
    printf("The absolute value of DBL_MIN equals %Le\n", fabsl(LDBL_MIN));
    printf("The maximum double floating number is %f\n", fmax(DBL_MIN, DBL_MAX));
    printf("The minimum double floating number is %e\n", fmin(DBL_MIN, DBL_MAX));
    printf("e mod 1/pi == %f\n", fmod(M_E, M_1_PI));
    printf("The smallest integer value not smaller than LDBL_MIN is %Lf\n", ceill(LDBL_MIN));
    printf("The largest integer value not greater than LDBL_MAX is %Lf\n", floorl(LDBL_MAX));

    /* <ctype.h> library */
    fflush(stdin);
    printf("input a charater: ");
    charjudge(getchar());
    return 0;
}