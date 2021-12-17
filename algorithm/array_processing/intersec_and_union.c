#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <string.h>

int64_t* get_union(const int64_t *arr1, size_t len1, const int64_t *arr2, size_t len2, size_t *len3)
{
    int64_t tmp1[len1], tmp2[len2], arr3[len1 + len2];
    size_t i, j, k;
    memcpy(tmp1, arr1, sizeof tmp1);
    memcpy(tmp2, arr2, sizeof tmp2);
    memset(arr3, 0, len1 + len2);
    /* sort tmp1 from small to large */
    for (size_t depth = 1; depth < len1; depth <<= 1 )
    for (size_t left = 0; left < len1; left += depth )
    {
        size_t right = ( left + depth < len1 ) ? left + depth - 1 : len1 - 1;
        size_t middle = ( left == right ) ? left : 1 + left + ( (right - left) >> 1 );
        for (i = left, j = middle + 1, k = 0; i <= middle && j <= right; k++)
            tmp1[k] = ( arr1[i] < arr1[j] ) ? arr1[i++] : arr1[j++];
        while ( i <= middle )
            tmp1[k++] = arr1[i++];
        while ( j <= right )
            tmp1[k++] = arr1[j++];
    }
    /* sort tmp2 from small to large */
    for (size_t depth = 1; depth < len2; depth <<= 1 )
    for (size_t left = 0; left < len2; left += depth )
    {
        size_t right = ( left + depth < len2 ) ? left + depth - 1 : len2 - 1;
        size_t middle = ( left == right ) ? left : 1 + left + ( (right - left) >> 1 );
        for (i = left, j = middle + 1, k = 0; i <= middle && j <= right; k++)
            tmp2[k] = ( arr2[i] < arr2[j] ) ? arr2[i++] : arr2[j++];
        while (i <= middle )
            tmp2[k++] = arr2[i++];
        while (j <= right)
            tmp2[k++] = arr2[j++];
    }
    /* get the union of tmp1 and tmp2 and assign the union to array3 */
    for ( i = 0, j = 0, *len3 = 0; i < len1 && j < len2;)
    {
        if ( tmp1[i] == tmp2[j] )
        {
            arr3[*len3++] = tmp1[i], i++, j++;
            continue;
        }
        arr3[*len3++] = ( tmp1[i] < tmp2[j] ) ? tmp1[i++] : tmp2[j++];
    }
    arr3 = (int64_t *)realloc(arr3, (*len3) * sizeof(int64_t));
    return arr3;
}

int64_t* get_intersec(const int64_t *arr1, size_t len1, const int64_t *arr2, size_t len2, size_t *len3)
{
    size_t i, j, k;
    int64_t tmp1[len1], tmp2[len2], arr3[len1 < len2 ? len1 : len2];
    memcpy(tmp1, arr1, sizeof tmp1);
    memcpy(tmp2, arr2, sizeof tmp2);
    memset(arr3, 0, sizeof(arr3));
    /* sort tmp1 from small to large */
    for ( size_t depth = 1; depth < len1; depth <<= 1 )
    for ( size_t left = 0; left < len1; left += depth )
    {
        size_t right = (left + depth < len1) ? left + depth - 1 : len1 - 1;
        size_t middle = ( left == right ) ? left : 1 + left + ( (right - left) >> 1 );
        for ( i = left, j = middle + 1, k = 0; i <= middle && j <= right; k++ )
        tmp1[k] = ( arr1[i] < arr1[j] ) ? arr1[i++] : arr1[j++];
        while (i <= middle )
            tmp1[k++] = arr1[i++];
        while (j <= right )
            tmp1[k++] = arr1[j++];
    }
    /* sort tmp2 from small to large */
    for (size_t depth = 1; depth < len2; depth <<= 1 )
    for (size_t left = 0; left < len2; left += depth )
    {
        size_t right = ( left + depth < len2 ) ? left + depth - 1 : len2 - 1;
        size_t middle = ( left == right ) ? left : 1 + left + ( (right - left) >> 1 );
        for ( i = left, j = middle + 1, k = 0; i <= middle && j <= right; k++)
            tmp2[k] = (arr2[i] < arr2[j]) ? arr2[i++] : arr2[j++];
        while (i <= middle)
            tmp2[k++] = arr2[i++];
        while (j <= right)
            tmp2[k++] = arr2[j++];
    }
    /* get the intersection of tmp1 and tmp2 and assign the intersection to array3 */
    for ( i = 0, j = 0, *len3 = 0; i < len1 && j < len2; i++, j++ )
        if ( tmp1[i] == tmp2[j] )
            arr3[*len3++] = tmp1[i];
    arr3 = (int64_t *)realloc(arr3, (*len3) * sizeof(int64_t));
    return arr3;
}