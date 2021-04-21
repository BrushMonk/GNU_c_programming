#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <inttypes.h>
#include <time.h>
#include <string.h>
#include <stddef.h>

static _Atomic(enum {left, right}) side;
struct AVL_node {
    int32_t node_id;
    struct AVL_node *next[2];
    /*balance factor */
    int_fast8_t bf;};
static _Atomic(ptrdiff_t) top = -1;
static struct AVL_node* volatile stack[SHRT_MAX];

/* pop up out of stack */
static struct AVL_node* popup(void)
{
    if (top == -1)
    {
        perror("popup() on the empty stack, abort");
        exit(-1);
    }
    return stack[top--];
}
/* push struct AVL_node stack */
static void push(struct AVL_node *node)
{
    if (top == SHRT_MAX)
    {
        perror("stack overflow");
        exit(-1);
    }
    stack[++top] = node;
}

struct AVL_node* look_up_a_key_in_AVL(struct AVL_node **const AVL_tree, int32_t key)
{
    struct AVL_node *cur = *AVL_tree;
    while (cur->node_id != key && cur)
    {
        side = (cur->node_id < key) ? left : right;
        cur = cur->next[side];
    }
    if (!cur) fprintf(stderr, "no key value in binary search tree!\n");
    return cur;
}

struct AVL_node* balance_a_node_in_AVL(struct AVL_node *node)
{
    side = (node->bf > 0) ? right : left;
    struct AVL_node *longer_child_node = node->next[side];
    if (node->bf * longer_child_node->bf < 0)
    {
        /* rotation for the longer child node of the current node,
        i.e, the first rotation for RL and LR. */
        node->next[side] = longer_child_node->next[!side];
        longer_child_node->next[!side] = node->next[side]->next[side];
        node->next[side]->next[side] = longer_child_node;
        longer_child_node->bf += ((node->bf > 0) ? 1 : -1) << ((node->bf * node->next[side]->bf < 0) ? 1 : 0);
        node->next[side]->bf = ((node->bf > 0) ? 1 : -1) << ((node->bf * node->next[side]->bf > 0) ? 1 : 0);
        longer_child_node = node->next[side];
    }
    /* rotation for the current node,
    i.e, the second rotation for RL and LR, or the rotation for LL and RR. */
    node->next[side] = longer_child_node->next[!side];
    longer_child_node->next[!side] = node;
    node->bf += ((node->bf > 0) ? -2 : 2) >> ((longer_child_node->bf & 1) ? 0 : 1);
    longer_child_node->bf = 0;
    node = longer_child_node;
    return node;
}

int insert_a_key_in_AVL(struct AVL_node **AVL_tree, int32_t new_key)
{
    if (*AVL_tree == NULL)
    {
        struct AVL_node *root = (struct AVL_node *)calloc(1, sizeof(struct AVL_node));
        root->node_id = new_key;
        root->next[left] = root->next[right] = NULL;
        root->bf = 0;
        *AVL_tree = root;
        printf("insert key value %" PRId32" successfully.\n", new_key);
        return 0;
    }
    struct AVL_node *cur = *AVL_tree;
    while (cur)
    {
        if (cur->node_id == new_key)
        {
            fprintf(stderr, "insert failed. This tree has already a node with key value %" PRId32".\n", new_key);
            top = -1; return -1;
        }
        push(cur);
        side = (cur->node_id<new_key) ? left : right;
        cur = cur->next[side];
    }
    struct AVL_node *new_node = (struct AVL_node *)calloc(1, sizeof(struct AVL_node));
    new_node->node_id = new_key;
    new_node->next[left] = new_node->next[right] = NULL;
    new_node->bf = 0; cur = new_node;
    while (top != -1)
    {
        stack[top]->bf += (cur == stack[top]->next[left]) ? 1:-1;
        cur = popup();
        if (cur->bf == 0)
            break;
        if (cur->bf < -1 || cur->bf > 1)
        {
            _Bool is_right_node = (cur == stack[top]->next[right]);
            cur = balance_a_node_in_AVL(cur);
            stack[top]->next[is_right_node] = cur;
            break;
        }
    }
    printf("insert key value %" PRId32" successfully.\n", new_key);
    top = -1; return 0;
}

int delete_a_key_and_fill_from_left_subtree_in_AVL(struct AVL_node **AVL_tree, int32_t key)
{
    struct AVL_node *cur = *AVL_tree;
    while (cur->node_id != key && cur)
    {
        push(cur);
        side = (cur->node_id < key) ? left:right;
        cur = cur->next[side];
    }
    if (!cur)
    {
        fprintf(stderr, "no key value in binary search tree!\n");
        top=-1; return -1;
    }
    /* delete a leaf node */
    if (cur->next[left] == NULL && cur->next[right] == NULL)
    {
        struct AVL_node *tmp = cur;
        cur = NULL;
        free(tmp);
    }
    /* use the maximal node in left subtree of the current node to replace the current node */
    else if (cur->next[left] != NULL && cur->next[right] != NULL)
    {
        push(cur);
        struct AVL_node *max_in_left_subtree = cur->next[left];
        while (max_in_left_subtree->next[right])
        {
            push(max_in_left_subtree);
            max_in_left_subtree = max_in_left_subtree->next[right];
        }
        cur->node_id = max_in_left_subtree->node_id;
        stack[top]->next[right] = max_in_left_subtree->next[left];
        free(max_in_left_subtree);
        while (stack[top] != cur)
        {
            stack[top]->bf++;
            popup();
        }
        cur = cur->next[left];
    }
    /* delete a node who has one subtree */
    else
    {
        struct AVL_node *tmp = cur;
        stack[top]->next[side] = (cur->next[left]) ? cur->next[left] : cur->next[right];
        cur = stack[top]->next[side];
        free(tmp);
        cur->bf = 0;
    }
    while (top != -1)
    {
        stack[top]->bf += (cur == stack[top]->next[left]) ? -1 : 1;
        cur = popup();
        if (cur->bf & 1)
            break;
        if (cur->bf < -1 || cur->bf > 1)
        {
            _Bool is_right_node = (cur == stack[top]->next[right]);
            cur = balance_a_node_in_AVL(cur);
            stack[top]->next[is_right_node] = cur;
        }
    }
    printf("delete key value %" PRId32" successfully.\n", key);
    top = -1; return 0;
}

int delete_a_key_and_fill_from_right_subtree_in_AVL(struct AVL_node **AVL_tree, int32_t key)
{
    struct AVL_node *cur = *AVL_tree;
    while (cur->node_id != key && cur)
    {
        push(cur);
        side = (cur->node_id < key) ? left : right;
        cur = cur->next[side];
    }
    if (!cur)
    {
        fprintf(stderr, "no key value in binary search tree!\n");
        top = -1; return -1;
    }
    /* delete a leave node */
    if (cur->next[left] == NULL && cur->next[right] == NULL)
    {
        struct AVL_node *tmp = cur;
        cur = NULL;
        free(tmp);
    }
    /* use the minimal node in rigth subtree of the current node to replace the current node */
    else if (cur->next[left] != NULL && cur->next[right] != NULL)
    {
        push(cur);
        struct AVL_node *min_in_right_subtree = cur->next[right];
        while (min_in_right_subtree->next[left])
        {
            push(min_in_right_subtree);
            min_in_right_subtree = min_in_right_subtree->next[left];
        }
        cur->node_id = min_in_right_subtree->node_id;
        stack[top]->next[left] = min_in_right_subtree->next[right];
        free(min_in_right_subtree);
        while (stack[top] != cur)
        {
            stack[top]->bf--;
            popup();
        }
        cur = cur->next[right];
    }
    /* delete a node who has one subtree */
    else
    {
        struct AVL_node *tmp = cur;
        stack[top]->next[side] = (cur->next[left]) ? cur->next[left] : cur->next[right];
        cur = stack[top]->next[side];
        free(tmp);
        cur->bf = 0;
    }
    while (top != -1)
    {
        stack[top]->bf += (cur == stack[top]->next[left]) ? -1 : 1;
        cur = popup();
        if (cur->bf & 1)
            break;
        if (cur->bf < -1 || cur->bf > 1)
        {
            _Bool is_right_node = (cur == stack[top]->next[right]);
            cur = balance_a_node_in_AVL(cur);
            stack[top]->next[is_right_node] = cur;
        }
    }
    printf("delete key value %" PRId32" successfully.\n", key);
    top = -1; return 0;
}

int32_t* Fisher_Yates_shuffle(int32_t *restrict array, size_t len, size_t shuffle_len)
{
    if (len < shuffle_len)
    {
        perror("The length of array overflows");
        exit(EXIT_FAILURE);
    }
    int32_t *shuffle;
    shuffle = (int32_t *)realloc(shuffle, shuffle_len * sizeof(size_t));
    memcpy(shuffle, array, shuffle_len * sizeof(size_t));
    for (size_t i = 0; i < shuffle_len; i++)
    {
        srand((unsigned)time(NULL));
        size_t random_num = rand() % (len - 1 - i) + i;
        int32_t tmp = shuffle[random_num]; shuffle[random_num] = shuffle[i]; shuffle[i] = tmp;
    }
    return shuffle;
}

int main(void)
{
    int32_t arr[INT_MAX];
    for (size_t i = 0; i < (size_t)INT_MAX; i++) arr[i] = (int32_t)i;
    int32_t *key_arr = Fisher_Yates_shuffle(arr, (size_t)INT_MAX, USHRT_MAX);
    struct AVL_node **AVL_tree;
    *AVL_tree = NULL;
    for (size_t i = 0; i < USHRT_MAX; i++)
        insert_a_key_in_AVL(AVL_tree, key_arr[i]);
    key_arr = Fisher_Yates_shuffle(key_arr, USHRT_MAX, USHRT_MAX);
    for (size_t i = 0; i < USHRT_MAX; i++)
    {
        delete_a_key_and_fill_from_left_subtree_in_AVL(AVL_tree, key_arr[i++]);
        delete_a_key_and_fill_from_right_subtree_in_AVL(AVL_tree, key_arr[i]);
    }
    *AVL_tree = NULL;
    return 0;
}