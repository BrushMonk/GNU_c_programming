#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <string.h>

int32_t* Fisher_Yates_shuffle(int32_t *restrict arr, size_t len, size_t shuffle_len)
{
    if (len < shuffle_len)
    {
        perror("The length of array overflows");
        exit(EXIT_FAILURE);
    }
    int32_t *shuffle = (int32_t *)realloc(shuffle, shuffle_len * sizeof(size_t));
    memcpy(shuffle, arr, shuffle_len * sizeof(size_t));
    for ( size_t i = 0; i < shuffle_len; i++ )
    {
        srand((unsigned)time(NULL));
        size_t random_num = rand() % (len - 1 - i) + i;
        int32_t tmp = shuffle[random_num]; shuffle[random_num] = shuffle[i]; shuffle[i] = tmp;
    }
    return shuffle;
}