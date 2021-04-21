#pragma once
#include <stdio.h>
#include <inttypes.h>
#include <stdlib.h>

void output_n_elem_combin(const int_fast64_t *arr, int_fast64_t *combin_arr,
size_t start, const size_t end, const size_t n, size_t i)
{
    if (i == n)
    {
        for (size_t k = 0; k < n; k++)
            printf("%" PRId64" ", combin_arr[k]);
        putchar('\n'); return;
    }
    for (; start <= end && end - start + 1 >= n - i; start++)
    /* "end-start+1 >= n-i" makes sure that including i at n */
    /* will make a combination with remaining elements at remaining positions. */
    {
        combin_arr[i] = arr[start];
        output_n_elem_combin(arr, combin_arr, start+1, end, n, i+1);
    }
}

size_t get_array(int_fast64_t **ptr_to_ptr)
{
    size_t len = 1;
    static char buf[BUFSIZ];
    void *alloc_check;
    fprintf(stdout, "Enter decimal int_fast64_t arrays(use spaces key to separate every number):\n");
    while ( fscanf(stdin, "%" SCNd64"", *ptr_to_ptr + (int_fast64_t)len - 1) == 1 )
    {
        if ( (*ptr_to_ptr = (int_fast64_t*)realloc(*ptr_to_ptr,
        (len + 1) * sizeof(int_fast64_t))) == NULL )
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

size_t get_size_t_integer(void)
{
    size_t number;
    _Bool input_check;
    fputs("Enter a size_t number: ", stdout);
    while ( (input_check = fscanf(stdin, "%zd", &number)) != 1 )
    if(input_check != 1)
    {
        scanf("%*s");
        fprintf(stderr, "invalid input, enter this number again: ");
    }
    return number;
}

int main(void)
{
    int_fast64_t *arr = (int_fast64_t*)calloc(1, sizeof(int_fast64_t));
    size_t len = get_array(&arr);
    size_t n = get_size_t_integer();
    int_fast64_t combin_arr[n];
    output_n_elem_combin(arr, combin_arr, 0, len - 1, n, 0);
    free(arr);
    exit(EXIT_SUCCESS);
}