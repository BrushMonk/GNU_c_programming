#include <stdlib.h>
#include <stdio.h>
#include <limits.h>
#include <inttypes.h>
#include <time.h>
struct skip_list_node {
    int32_t key_value;
    int *file_dscrptor;
    struct skip_list_node **next;};

void init_skip_list(struct skip_list_node **head_to_skip_list, size_t n)
{

}