#pragma once
#include <inttypes.h>
#include <stddef.h>
/* Given (possibly negative) integers a[1], a[2], ..., a[n],
find the maximum value of the sum of consecutive members,
that is, make a[i]+a[i+1]+...+a[j] to be maximum, 0<=i && i<j && j<=n. */
int_fast64_t max_sum_of_subsq(int_fast64_t *restrict arr, size_t len)
{
    int_fast64_t subsq_sum, max_sum;
    subsq_sum = max_sum = 0;
    for ( size_t i = 0; i < len; i++ )
    {
        subsq_sum += arr[i];
        if (subsq_sum > max_sum) max_sum = subsq_sum;
        if (subsq_sum < 0) subsq_sum = 0;
    }
    /* reset subsequence_sum to be 0 if subsq_sum is negative */
    return max_sum;
}