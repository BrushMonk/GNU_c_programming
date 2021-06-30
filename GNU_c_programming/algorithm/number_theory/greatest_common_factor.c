#pragma once
#include <inttypes.h>
uint64_t greatest_common_factor_via_differ(uint64_t a, uint64_t b)
{
    uint64_t differ;
    if(b > a)
    {
        uint64_t tmp = a; a = b; b = tmp;
    }
    while(a != b)
    {
        differ = a - b; a = b; b = differ;
    }
    return a;
}

uint64_t greatest_common_factor_via_divide_and_conquer(uint64_t a, uint64_t b)
{
    uint8_t power_of_2 = 0;
    if (a == 0) return b;
    if (b == 0) return a;
    while ( ((a | b) & 1) == 0 )
    /* when both a and b are even */
    {
        power_of_2++; a >>= 1; b >>= 1;
        /* a=a/2, b=b/2 */
    }
    while ((b & 1) == 0) b >>= 1/* when b is even, b=b/2 */;
    do {
        while ((a & 1) == 0) a >>= 1/* when a is even, a=a/2 */;
        if (b > a)
        {
            uint64_t tmp = a; a = b; b = tmp;
        }
        a -= b/* when both a and b are odd, a=a-b. a is even now. */;
    } while (a != b);
    return b << power_of_2/* return b*power(2, power_of_2) */;
}

uint64_t greatest_common_factor_via_remainder(uint64_t a, uint64_t b)
{
    uint64_t remainder;
    if (b > a)
    {
        uint64_t tmp = a; a = b; b = tmp;
    }
    while (b > 0)
    {
        remainder = a % b; a = b; b = remainder;
    }
    return a;
}