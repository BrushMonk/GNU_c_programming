#pragma once
#include <stdbool.h>
#include <string.h>
#include <inttypes.h>
#include <stdlib.h>
uint_least64_t* Euler_sieve(uint_least64_t n, uint_least64_t *ptr_to_number_of_prime)
{
    bool prime_mark[n + 1];
    uint_least64_t *prime = (uint_least64_t *)malloc((n + 1) * sizeof(uint_least64_t));
    uint_least64_t prime_counter = 0, i;
    memset(prime_mark, true, sizeof(prime_mark));
    for (i = 2; i <= n; i++)
    /* i serves as a counter until n. */
    {
        if (prime_mark[i])
            prime[prime_counter++] = i;
        for (uint_least64_t j = 0; j < prime_counter && prime[j] * i <= n; j++)
        {
            prime_mark[prime[j] * i] = false;
            /* use Fundamental theorem of arithmetic to sieve the composite number */
            if (i % prime[j] == 0) break;
            /* it means that if i is a composite number and */
            /* has the minimum prime divisor prime[s], */
            /* prime[j+1]*i must be sieved repeatedly later */
            /* because there will be prime[s]*t == prime[j+1]*i and t>i and s<j+1. */
        }
    }
    prime = (uint_least64_t *)realloc(prime, (prime_counter + 1) * sizeof(uint_least64_t));
    *ptr_to_number_of_prime = prime_counter + 1;
    return prime;
}