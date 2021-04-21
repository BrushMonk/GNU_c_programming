// n people stand in a circle waiting to be executed.
// Counting begins at a specified point in the circle from 1.
// After k−1 people are skipped, the k th person get executed.
// Then starting with next person from 1, after k−1 people are skipped,
// the k th person get executed. It is going on until the last one remained.
// Let the Josephus(n,k) denote the position of the survivor
// when there are n people and 1 is the start point of first round.
#pragma once
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
struct circ_node {
    int32_t seq;
    struct circ_node *next;};

void init_circ_list(struct circ_node **head_to_circ_list,
int32_t len)
{
    *head_to_circ_list = NULL;
    struct circ_node *former, *tmp, *current;
    for (int32_t i = 0; i < len; i++)
    {
        current = (struct circ_node *)malloc(sizeof(struct circ_node));
        /* Josephus problem count from 1. */
        current->seq = i + 1;
        /* allocates sizeof(struct circ_node) bytes */
        /* in the midst of uninitialized memory. */
        if (*head_to_circ_list == NULL)
            *head_to_circ_list = current;
        /* assign the first allocated momery of a pointer */
        /* to structure circ_node to *head_to_circ_list. */
        else
        {
            former = tmp;
            former->next = current;
        }
        /* assign the non-first allocated memory of a pointer */
        /* to structure circ_node to former->next. */
        if (i < len - 1) current->next = NULL;
        else current->next = *head_to_circ_list;
        /* set start->next to be NULL. */
        tmp = current;
    }
    return;
}

struct circ_node* the_1st_Josephus_solution(struct circ_node **head_to_circ_list,
int32_t k)
{
    struct circ_node *current, *tmp;
    current->next = *head_to_circ_list;
    while (current->next != current)
    {
        for (int32_t i = 0; i < k - 1; i++, current = current->next);
        /* don't current->next=current, which is self reference. */
        printf("%" PRId32" has been removed.\n", current->seq);
        tmp = current->next;
        current->next = tmp->next;
        free(tmp);
    }
    printf("%" PRId32" is left in the end.\n", current->seq);
    return current;
}

struct circ_node* the_2rd_Josephus_solution(struct circ_node **head_to_circ_list,
int32_t len, int32_t k)
{
    struct circ_node *current;
    current->next = *head_to_circ_list;
    int32_t Josephus = 1;
    for (int32_t i = 1; i < len; i++ )
        Josephus = ( (Josephus + k - 1) % (i + 1) + i + 1 ) % (i + 1) + 1;
        /* Josephus(n,k)=(Josephus(n−1,k)+k−1)mod n + 1 */
    for (int32_t i = 0; i < Josephus; i++, current = current->next);
    printf("%" PRId32" is left in the end.\n", current->seq);
    return current;
}