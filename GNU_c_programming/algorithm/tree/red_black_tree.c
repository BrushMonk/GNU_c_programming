#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#define red 1
#define black 0
static _Atomic(enum {left, right}) side;
struct RB_node {
    int64_t node_id;
    _Bool color;
    struct RB_node *next[2], *parent;};

struct RB_node* look_up_a_key_in_RBT(struct RB_node **const RB_tree, int64_t key)
{
    struct RB_node *cur = *RB_tree;
    while (cur->node_id != key && cur)
    {
        side = (cur->node_id < key) ? left : right;
        cur = cur->next[side];
    }
    if (!cur) fputs("no key value in binary search tree!\n", stderr);
    return cur;
}

struct RB_node* balance_and_recolor_a_node_in_RBT_after_insert(struct RB_node *node)
{
    side = (node->next[left]->color == red) ? left : right;
    struct RB_node *red_child_node = node->next[side];
    if (node->next[side]->next[!side]->color == red)
    {
        /* rotation for the longer child node of the current node, */
        /* i.e, the first rotation for RL and LR. */
        node->next[side] = red_child_node->next[!side];
        node->next[side]->parent = node;
        red_child_node->next[!side] = node->next[side]->next[side];
        red_child_node->next[!side]->parent = red_child_node;
        red_child_node->parent = node->next[side];
        node->next[side]->next[side] = red_child_node;
        red_child_node = node->next[side];
    }
    /* rotation for the current node, */
    /* i.e, the second rotation for RL and LR, or the rotation for LL and RR. */
    node->next[side] = red_child_node->next[!side];
    node->next[side]->parent = red_child_node;
    red_child_node->next[!side] = node;
    red_child_node->parent = node->parent;
    node->parent->next[node->parent->next[right] == node] = red_child_node;
    node->parent = red_child_node;
    red_child_node->color = black;
    node->color = red;
    node = red_child_node;
    return node;
}

int insert_a_key_in_RBT(struct RB_node **RB_tree, int64_t new_key)
{
    if (*RB_tree == NULL)
    {
        struct RB_node *root = (struct RB_node *)calloc(1, sizeof(struct RB_node));
        root->node_id = new_key;
        root->next[left] = root->next[right] = root->parent = NULL;
        root->color = black;
        *RB_tree = root;
        printf("insert key value %" PRId64" successfully.\n", new_key);
        return 0;
    }
    struct RB_node *cur=*RB_tree, *previous_of_cur;
    while (cur)
    {
        if (cur->node_id == new_key)
        {
            fprintf(stderr, "insert failed. This tree has already a node with key value %" PRId64".\n", new_key);
            return -1;
        }
        previous_of_cur = cur;
        side = (cur->node_id < new_key) ? left : right;
        cur = cur->next[side];
    }
    struct RB_node *new_node = (struct RB_node *)calloc(1, sizeof(struct RB_node));
    new_node->node_id = new_key;
    new_node->next[left] = new_node->next[right] = NULL;
    new_node->parent = previous_of_cur;
    new_node->color = red; cur = new_node;
    if (cur->parent->color == black) return 0;
    while (cur->parent->color == red)
    {
        side = (cur->parent->parent->next[left] == cur->parent) ? right : left;
        struct RB_node *uncle_node = cur->parent->parent->next[side];
        if (uncle_node->color == red)
        {
            uncle_node->color = cur->parent->color = black;
            cur->parent->parent->color = red;
        }
        else cur->parent->parent = balance_and_recolor_a_node_in_RBT_after_insert(cur->parent->parent);
        cur = cur->parent;
    }
    printf("insert key value %" PRId64" successfully.\n", new_key);
    return 0;
}

struct RB_node* delete_a_node_with_one_child_in_BST(struct RB_node *node)
{
    struct RB_node *child_node;
    child_node = (node->next[left]) ? node->next[left] : node->next[right];
    child_node->parent = node->parent;
    node->parent->next[side] = child_node;
    free(node);
    return child_node;
}

struct RB_node* balance_and_recolor_a_black_node_in_RBT_after_deleting_a_black_node(struct RB_node *parent_of_node)
{
    struct RB_node *sibling_node = parent_of_node->next[!side];
    _Bool nephew_color_in_same_side, nephew_color_in_another_side;
    nephew_color_in_same_side = (sibling_node->next[side]->color == black || !sibling_node->next[side]) ? black : red;
    nephew_color_in_another_side = (sibling_node->next[!side]->color == black || !sibling_node->next[!side]) ? black : red;
    if (nephew_color_in_same_side | nephew_color_in_another_side | sibling_node->color | parent_of_node->color)
    {
        if (sibling_node->color == red)
        {
            parent_of_node->next[side] = sibling_node->next[!side];
            parent_of_node->next[side]->parent = sibling_node;
            sibling_node->next[!side] = parent_of_node;
            sibling_node->parent = parent_of_node->parent;
            if (!parent_of_node->parent)
                parent_of_node->parent->next[parent_of_node->parent->next[right] == parent_of_node] = sibling_node;
            parent_of_node->parent = sibling_node;
            parent_of_node->color = red;
            sibling_node->color = black;
            parent_of_node = balance_and_recolor_a_black_node_in_RBT_after_deleting_a_black_node(parent_of_node);
        }
        else if (nephew_color_in_another_side == red)
        {
            struct RB_node *left_nephew_node = sibling_node->next[!side];
            parent_of_node->next[side] = left_nephew_node;
            left_nephew_node->parent = parent_of_node;
            sibling_node->next[!side] = left_nephew_node->next[side];
            sibling_node->next[!side]->parent = sibling_node;
            left_nephew_node->next[side] = sibling_node;
            sibling_node->parent = left_nephew_node;
            parent_of_node = balance_and_recolor_a_black_node_in_RBT_after_deleting_a_black_node(parent_of_node);
        }
        else if (nephew_color_in_same_side == red)
        {
            parent_of_node->next[side] = sibling_node->next[!side];
            parent_of_node->next[side]->parent = sibling_node;
            sibling_node->next[!side] = parent_of_node;
            sibling_node->parent = parent_of_node->parent;
            if (!parent_of_node->parent)
                parent_of_node->parent->next[parent_of_node->parent->next[right] == parent_of_node] = sibling_node;
            parent_of_node->parent = sibling_node;
            _Bool tmp = sibling_node->color; sibling_node->color = parent_of_node->color; parent_of_node->color = tmp;
            parent_of_node = sibling_node;
            parent_of_node->next[side]->color = black;
        }
        else {
            sibling_node->color = red;
            parent_of_node->color = black;
        }
    }
    else {
        sibling_node->color = red;
        if (!parent_of_node->parent)
        {
            side = (parent_of_node->parent->next[left] == parent_of_node) ? left : right;
            parent_of_node->parent = balance_and_recolor_a_black_node_in_RBT_after_deleting_a_black_node(parent_of_node->parent);
        }
    }
    return parent_of_node;
}

int delete_a_key_and_fill_from_left_subtree_in_RBT(struct RB_node **RB_tree, int64_t key)
{
    struct RB_node *cur = look_up_a_key_in_RBT(RB_tree, key);
    if (!cur) return -1;
    struct RB_node *previous_of_cur = cur->parent;
    _Bool deleted_color = cur->color;
    if (cur->next[left] == NULL && cur->next[right] == NULL)
        struct RB_node *tmp = cur, cur = NULL, free(tmp);
    /* use the maximal node in left subtree of the current node to replace the current node */
    else if (cur->next[left] != NULL && cur->next[right] != NULL)
    {
        struct RB_node *max_in_left_subtree = cur->next[left];
        while (max_in_left_subtree->next[right])
            max_in_left_subtree = max_in_left_subtree->next[right];
        cur->node_id = max_in_left_subtree->node_id;
        cur = max_in_left_subtree;
        previous_of_cur = max_in_left_subtree->parent;
        deleted_color = max_in_left_subtree->color;
        side = right;
        cur = delete_a_node_with_one_child_in_BST(cur);
    }
    else cur = delete_a_node_with_one_child_in_BST(cur);
    if (deleted_color == red);
    else if (deleted_color == black && cur->color == red)
        cur->color = black;
    else previous_of_cur = balance_and_recolor_a_black_node_in_RBT_after_deleting_a_black_node(previous_of_cur);
    printf("delete key value %" PRId64" successfully.\n", key);
    return 0;
}

int delete_a_key_and_fill_from_right_subtree_in_RBT(struct RB_node **RB_tree, int64_t key)
{
    struct RB_node *cur = look_up_a_key_in_RBT(RB_tree, key);
    if (!cur) return -1;
    struct RB_node *previous_of_cur = cur->parent;
    _Bool deleted_color = cur->color;
    if (cur->next[left] == NULL && cur->next[right] == NULL)
        struct RB_node *tmp = cur, cur = NULL, free(tmp);
    /* use the minimal node in right subtree of the current node to replace the current node */
    else if (cur->next[left] != NULL && cur->next[right] != NULL)
    {
        struct RB_node *min_in_right_subtree = cur->next[right];
        while (min_in_right_subtree->next[left])
            min_in_right_subtree = min_in_right_subtree->next[left];
        cur->node_id = min_in_right_subtree->node_id;
        cur = min_in_right_subtree;
        previous_of_cur = min_in_right_subtree->parent;
        deleted_color = min_in_right_subtree->color;
        side = left;
        cur = delete_a_node_with_one_child_in_BST(cur);
    }
    else cur = delete_a_node_with_one_child_in_BST(cur);
    if (deleted_color == red);
    else if (deleted_color == black && cur->color == red)
        cur->color = black;
    else previous_of_cur = balance_and_recolor_a_black_node_in_RBT_after_deleting_a_black_node(previous_of_cur);
    printf("delete key value %" PRId64" successfully.\n", key);
    return 0;
}