#include <stdio.h>
#include <inttypes.h>

static void compare_signed_and_unsigned(int64_t x, uint64_t benchmark)
/* when comparing unsigned type and signed type, */
/* computer first convert signed type into unsigned type in binary, */
/* i.e, signed n is negative, n goes 2^32+n, */
/* then computer compares them. */
{
    if (x < benchmark) puts("is less than benchmark.");
    else if (x == benchmark) puts("is equal to benchmark.");
    else if (x > benchmark) puts("is more than benchmark.");
}

int main(void)
{
    int64_t a = -7, b = -4, c = -5;
    /* signed type is stored as the two’s complement binary number. */
    /* so int -1 is stored as 1111 1111 1111 1111. */
    uint64_t benchmark = 5;
    printf("a equals %" PRId64".\n"
    "b equals %" PRId64".\n"
    "c equals %" PRId64".\n"
    "benchmark equals %" PRIu64".\n",
    a, b, c, benchmark);
    printf("a "); compare_signed_and_unsigned(a, benchmark);
    printf("b "); compare_signed_and_unsigned(b, benchmark);
    printf("c "); compare_signed_and_unsigned(c, benchmark);
    printf("a-benchmark==%" PRId64".\nb-benchmark==%" PRId64".\nc-benchmark==%" PRId64".\n",
    a - benchmark, b - benchmark, c - benchmark);
    /* when calculating unsigned type and signed type, */
    /* computer first convert signed type into unsigned type in binary, */
    /* i.e, signed n is negative, n goes 2^32+n, */
    /* then computer makes calculation. */
    return 0;
}