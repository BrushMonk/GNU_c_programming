#include <stdlib.h>
#include <stdio.h>
#include <limits.h>
#include <inttypes.h>
#include <time.h>
struct skip_node {
    int32_t key_value;
    int *fd;
    struct skip_node *next;
    struct skip_node *skip_to;};

void init_skip_list(struct skip_node *head_to_skip_list, size_t n)
{

}