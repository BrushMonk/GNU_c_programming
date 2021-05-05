#pragma once
#include <stdio.h>
#include <inttypes.h>
#include <stdlib.h>
#include <stddef.h>

uint_least64_t derange(int_fast8_t *restrict arr, const ptrdiff_t len,
_Bool isdisplay, int_fast8_t *restrict Peano, ptrdiff_t depth)
{
    uint_least64_t count = 0;
    if (depth == len)
    {
        if (isdisplay)
        {
            for ( ptrdiff_t i = 0; i < len; i++ )
            fprintf(stdout, "%" PRId8"\040", arr[i]);
            putchar('\n');
        }
        return 1;
    }
    for ( ptrdiff_t i = len - 1; i >= depth; i-- )
    {
        if ( i == Peano[depth] ) continue;
        int_fast8_t tmp = Peano[i]; Peano[i] = Peano[depth]; Peano[depth] = tmp;
        tmp = arr[i]; arr[i] = arr[depth]; arr[depth] = tmp;
        count += derange(arr, len, isdisplay, Peano, depth + 1);
        tmp = Peano[i]; Peano[i] = Peano[depth]; Peano[depth] = tmp;
        tmp = arr[i]; arr[i] = arr[depth]; arr[depth] = tmp;
    }
    return count;
}

size_t get_array(int_fast8_t **ptr_to_ptr)
{
    size_t len = 1;
    static char buf[BUFSIZ];
    void *alloc_check;
    fprintf(stdout, "Enter decimal int_fast8_t arrays(use spaces key to separate every number):\n");
    while ( fscanf(stdin, "%" SCNd8"", *ptr_to_ptr + (int_fast8_t)len - 1) == 1 )
    {
        if( (*ptr_to_ptr = (int_fast8_t*)realloc(*ptr_to_ptr,
        (len + 1) * sizeof(int_fast8_t))) == NULL )
        {
            perror("fail to allocate the input array");
            exit(EXIT_FAILURE);
        }
        len++;
        if (getchar() == '\n') break;
    }
    setbuf(stdin, NULL);
    setvbuf(stdin, buf, _IOLBF, BUFSIZ);
    /* skip rest of input content */
    return len;
}

int main(void)
{
    int_fast8_t *arr = (int_fast8_t*)calloc(1, sizeof(int_fast8_t));
    size_t len = get_array( &arr );
    int_fast8_t Peano[len];
    for (int_fast8_t i = 0; i < (int_fast8_t)len; i++) Peano[i] = i;
    puts("Here are all derangements for this integer array:");
    derange(arr, len, 1, Peano, 0);
    printf("The derangements of the array are total of %" PRIu64".\n", derange(arr, len, 0, Peano, 0));
    free(arr);
    exit(EXIT_SUCCESS);
}