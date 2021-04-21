#pragma once
#include <stdio.h>
#include <limits.h>
#include <stdlib.h>
#include <inttypes.h>
#include <time.h>
int main(void)
{
    int32_t random_number, cursor;
    int64_t max, min;
    fprintf(stdout, "please enter the integer maximum(between the range from %d to %d): ", INT_MIN, INT_MAX);
    while ( fscanf(stdin, "%ld", &max) != 1 || (uint_fast64_t)max <= (uint_fast64_t)INT_MIN || (uint_fast64_t)max > (uint_fast64_t)INT_MAX )
    {        
        while ( getchar() != '\n' ) continue;/* skip rest of input content */
        fprintf(stderr, "invalid maximum value, enter again: ");
        continue;
    }
    while ( getchar() != '\n' ) continue;/* skip rest of input content */
    fprintf(stdout, "please enter the integer minimum(between the range from %d to %d): ", INT_MIN, (int32_t)max);
    while ( fscanf(stdin, "%ld", &min) != 1 || (int_fast64_t) (((uint_fast64_t)INT_MIN - (uint_fast64_t)min) & ((uint_fast64_t)min - (uint_fast64_t)max)) > 0)
    /* the statement (int_fast64_t)(((uint_fast64_t)INT_MIN - (uint_fast64_t)min) & ((uint_fast64_t)min - (uint_fast64_t)max))>0 equals min<INT_MIN or min>max */
    {
        while ( getchar() != '\n' )continue;/* skip rest of input content */
        fprintf(stderr, "invalid minimum value, enter again: ");
        continue;
    }
    while ( getchar() != '\n' ) continue;/* skip rest of input content */
    srand((unsigned)time(NULL));
    random_number = rand() % ((int32_t)max - (int32_t)min) + (int32_t)min;
    cursor = random_number + 1;
    while (cursor != random_number)
    {
        fprintf(stdout, "please guess the integer random number between the %d and %d: ", (int32_t)min, (int32_t)max);
        while ( fscanf(stdin, "%d", &cursor) != 1 || (int_fast64_t) (((uint_fast64_t)min - (uint_fast64_t)cursor) & ((uint_fast64_t)cursor - (uint_fast64_t)max)) > 0 )
            /* the statement (int_fast64_t)(((uint_fast64_t)min - (uint_fast64_t)cursor) & ((uint_fast64_t)cursor - (uint_fast64_t)max))>0 equals cursor<min or cursor>max */
            {
                while ( getchar() != '\n' ) continue;/* skip rest of input content */
                fprintf(stderr, "invalid cursor value or out of the range, enter again: ");
                continue;
            }
        while ( getchar() != '\n' )continue;/* skip rest of input content */
        min = (random_number > cursor) ? cursor : min;
        max = (random_number < cursor) ? cursor : max;
    }
    fprintf(stdout, "Bingo! you got the number! Game over!\n");
    exit(EXIT_SUCCESS);
}