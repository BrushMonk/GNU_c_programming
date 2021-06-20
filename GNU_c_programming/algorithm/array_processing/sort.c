#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include <stddef.h>

/* the complexity of merge sort is O(nlog n) */
void merge_sort_via_recur(int32_t *restrict arr, size_t left, size_t right)
{
    if (left == right) return;
    size_t middle = 1 + left + ( (right - left) >> 1 );
    merge_sort_via_recur(arr, left, middle);
    merge_sort_via_recur(arr, middle+1, right);

    int32_t *sub_arr=(int32_t *)calloc(1, (right-left + 1) * sizeof(int32_t));
    size_t i, j, k;
    for ( i = left, j = middle + 1, k = 0; i <= middle && j <= right; k++ )
        *(sub_arr + k) = (*(arr + i) < *(arr + j)) ? *(arr + i++) : *(arr + j++);
    while (i <= middle)
        *(sub_arr + k++) = *(arr + i++);
    while (j <= right)
        *(sub_arr + k++) = *(arr + j++);
    for ( k = 0; k <= right - left; k++ )
        *(arr + k + left) = *(sub_arr + k);
    free(sub_arr);
    return;
}
/* the complexity of merge sort is O(nlog n) */
void merge_sort_via_nested_loop(int32_t *restrict arr, size_t len)
{
    int32_t sub_arr[len];
    memcpy(sub_arr, arr, len * sizeof(int32_t));

    for (size_t depth = 1; depth < len; depth <<= 1)
    for (size_t left = 0; left < len; left += depth)
    {
        size_t right = (left + depth < len) ? left + depth - 1 : len - 1;
        size_t middle = (left == right) ? left : 1 + left + ((right - left) >> 1);
        size_t i, j, k;
        for ( i = left, j = middle + 1, k = 0; i <= middle && j <= right; k++)
        arr[k] = (sub_arr[i] < sub_arr[j]) ? sub_arr[i++] : sub_arr[j++];
        while (i <= middle)
        arr[k++] = sub_arr[i++];
        while (j <= right)
        arr[k++] = sub_arr[j++];
    }
    return;
}

/* the complexity of insertion sort is O(n^2) */
void insertion_sort(int32_t *restrict arr, ptrdiff_t len)
{
    for(ptrdiff_t i = 0; i < len; i++)
    /* pass from the first to the len th */
    for(ptrdiff_t j = i + 1; j >= 1; j--)
    {
        /* compare and swap each adjacent two members from the i+1 th to the second */
        if (arr[j] < arr[j-1])
        {
            int32_t tmp = arr[j]; arr[j] = arr[j-1]; arr[j-1] = tmp;
        }
        else break;
    }
    /* if sorting arr[i+1] is done, execute next imput arr[i+2] */
    return;
}

/* the complexity of selection sort is O(n^2) */
void selection_sort(int32_t **ptr_to_ptr, size_t len)
{
    int32_t *tmp;
    for (size_t i = 0; i < len; i++)
    /* pass from the first to the last */
    for (size_t j = i; j < len - 1; j++)
        /* compare and swap each adjacent two members from the i th to the len-1 th */
        *(tmp + i) = (*(*ptr_to_ptr + j) < *(*ptr_to_ptr + j + 1)) ? *(*ptr_to_ptr + j) : *(*ptr_to_ptr + j + 1);
    free(*ptr_to_ptr);
    *ptr_to_ptr = tmp;
    return;
}

/* binary maximum heap is a complete binary tree
where any node is larger than its child node */
void heap_sort(int32_t *restrict arr, int len)
{
    /* initialize i as the last nonleaf node in tree */
    for (int i = len >> 1 ; i >= 0; i--)
    /* the complexity of this procedure is O(n) */
    {
        int min_child = (i << 1) + 1;
        if (min_child + 1 < len && arr[min_child] < arr[min_child + 1])
            min_child++;
        if (arr[i] >= arr[min_child]) continue;
        else
        {
            int tmp = arr[i];
            arr[i] = arr[min_child];
            arr[min_child] = tmp;
        }
    }
    for (int i = len - 1; i > 0; i--)
    /* the complexity of this procedure is O(nlog n) */
    {
        int tmp = arr[i];
        arr[i] = arr[0];
        arr[0] = tmp;
        for (int cur = i, max_child = (cur << 1) + 1; max_child < len;)
        /* heapify from this first element. The complexity of this procedure is O(log n). */
        {
            if (max_child + 1 < len && arr[max_child] < arr[max_child + 1])
                max_child++;
            if (arr[cur] >= arr[max_child]) break;
            else
            {
                int tmp = arr[cur]; arr[cur] = arr[max_child]; arr[max_child] = tmp;
                cur = max_child;
                max_child = 2 * cur + 1;
            }
        }
    }
    return;
}