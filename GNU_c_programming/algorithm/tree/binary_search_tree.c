#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <stddef.h>
#include <limits.h>
struct bin_node {
    int32_t node_id;
    struct bin_node *left, *right;};

// *stack[SHRT_MAX] is an array consist of pointers.
static struct bin_node* volatile stack[SHRT_MAX];
static _Atomic(ptrdiff_t) top = -1;
/* pop up out of stack */
static struct bin_node* popup(void)
{
    if (top == -1){
    perror("popup() on the empty stack, abort");
    exit(-1);}
    return stack[top--];
}
/* push into stack */
static void push(struct bin_node *node)
{
    if (top == SHRT_MAX){
    perror("stack overflow");
    exit(-1);}
    stack[++top] = node;
}

struct bin_node* init_bin_root(int32_t root_key)
{
    struct bin_node *root = (struct bin_node *)calloc(1, sizeof(struct bin_node));
    root->node_id = root_key;
    root->right = root->left = NULL;
    return root;
}

int insert_a_node_in_BST(struct bin_node **BST, int32_t new_key)
{
    struct bin_node *cur = *BST;
    while (cur)
    {
        if (cur->node_id == new_key)
        {
            fprintf(stderr, "insert failed. This tree has already a node with key value %" PRId64".\n", new_key);
            return -1;
        }
        else cur = (cur->node_id < new_key) ? cur->left : cur->right;
    }
    struct bin_node *new_node = (struct bin_node *)calloc(1, sizeof(struct bin_node));
    new_node->node_id = new_key;
    new_node->left = new_node->right = NULL;
    cur = new_node;
    return 0;
}

struct bin_node* look_up_a_node_in_BST(const struct bin_node **BST, int32_t key)
{
    struct bin_node *cur = *BST;
    while (cur->node_id != key && cur)
    cur = (cur->node_id < key) ? cur->left : cur->right;
    if (!cur) fprintf(stderr, "no key value in binary search tree!\n");
    return cur;
}

int delete_a_node_and_fill_from_left_subtree_in_BST(struct bin_node **BST, int32_t key)
{
    struct bin_node *cur = *BST, *prev_of_cur;
    while (cur->node_id != key && cur)
    {
        prev_of_cur = cur;
        cur = (cur->node_id < key) ? cur->left : cur->right;
    }
    if (!cur)
    {
        fprintf(stderr, "no key value in binary search tree!\n");
        return -1;
    }
    /* delete a leave node */
    if (cur->left == NULL && cur->right == NULL)
    {
        struct bin_node *tmp = cur;
        cur = NULL;
        free(tmp); return 0;
    }
    /* use the maximum node in left subtree of the current node to replace the current node */
    else if (cur->left != NULL && cur->right != NULL)
    {
        struct bin_node *max_in_left_subtree = cur->left, *prev_of_max = cur;
        while (max_in_left_subtree->right)
        {
            prev_of_max = max_in_left_subtree;
            max_in_left_subtree = max_in_left_subtree->right;
        }
        cur->node_id = max_in_left_subtree->node_id;
        prev_of_max->right = max_in_left_subtree->left;
        free(max_in_left_subtree);
        return 0;
    }
    /* delete a node who has one subtree */
    else {
        struct bin_node *tmp = cur;
        if (cur->left && prev_of_cur->left == cur)
            prev_of_cur->left = cur->left;
        if (cur->left && prev_of_cur->right == cur)
            prev_of_cur->left = cur->right;
        if (cur->right && prev_of_cur->left == cur)
            prev_of_cur->right = cur->left;
        if (cur->right && prev_of_cur->right == cur)
            prev_of_cur->right = cur->right;
        free(tmp);
        return 0;
    }
}

int delete_a_node_and_fill_from_right_subtree_in_BST(struct bin_node **BST, int32_t key)
{
    struct bin_node *cur = *BST, *prev_of_cur;
    while (cur->node_id != key && cur)
    {
        prev_of_cur = cur;
        cur = (cur->node_id < key)? cur->left : cur->right;
    }
    if (!cur)
    {
        fprintf(stderr, "no key value in binary search tree!\n");
        return -1;
    }
    /* delete a leaf node */
    if (cur->left == NULL && cur->right == NULL)
    {
        struct bin_node *tmp = cur;
        cur = NULL;
        free(tmp);
        return 0;
    }
    /* use the minimum node in right subtree of the current node to replace the current node */
    else if (cur->left != NULL && cur->right != NULL)
    {
        struct bin_node *min_in_right_subtree = cur->right, *prev_of_min = cur;
        while (min_in_right_subtree->left)
        {
            prev_of_min = min_in_right_subtree;
            min_in_right_subtree = min_in_right_subtree->left;
        }
        cur->node_id = min_in_right_subtree->node_id;
        prev_of_min->left = min_in_right_subtree->right;
        free(min_in_right_subtree);
        return 0;
    }
    /* delete a node who has one subtree */
    else {
        struct bin_node *tmp = cur;
        if (cur->left && prev_of_cur->left == cur)
            prev_of_cur->left = cur->left;
        if (cur->left && prev_of_cur->right == cur)
            prev_of_cur->left = cur->right;
        if (cur->right && prev_of_cur->left == cur)
            prev_of_cur->right = cur->left;
        if (cur->right && prev_of_cur->right == cur)
            prev_of_cur->right = cur->right;
        free(tmp);
        return 0;
    }
}

void the_1st_preorder_trav_to_BST(struct bin_node **BST)
{
    struct bin_node *cur = *BST;
    if (cur == NULL) return;
    /* push the root node into the stack */
    push(cur);
    while (top != -1)
    {
        /* popup every node on top of the stack */
        cur = popup();
        printf("%" PRId32"\040", cur->node_id);
        /* first push right then left.
        Because it makes it sure that every left node pop up first,
        right node underneath its sibling left node will be popped up next time. */
        if (cur->right != NULL) push(cur->right);
        if (cur->left != NULL) push(cur->left);
    }
    return;
}

void the_2nd_preorder_trav_to_BST(struct bin_node **BST)
{
    struct bin_node *cur = *BST;
    if (cur == NULL) return;
    while (top != -1 || cur != NULL)
    {
        /* when stack is empty and current is NULL, loop done.
        Because when it traverses to the root node,
        stack is empty but cur == (*BST)->right. */
        if (cur != NULL)
        {
            printf("%" PRId32"\040", cur->node_id);
            /* use the node data first */
            push(cur);
            cur = cur->left;
        }
        else {
            cur = popup();
            cur = cur->right;
        }
    }
    return;
}

void inorder_trav_to_BST(struct bin_node **BST)
{
    struct bin_node *cur = *BST;
    if (cur == NULL) return;
    while (top != -1 || cur != NULL)
    {
        /* when stack is empty and current is NULL,
        loop done. Because when it traverses to the root node,
        stack is empty but cur == (*BST)->right. */
        if (cur != NULL)
        {
            push(cur);
            cur = cur->left;
        }
        else {
            cur = popup();
            printf("%" PRId32"\040", cur->node_id);
            cur = cur->right;
        }
    }
    return;
}

void postorder_trav_to_BST(struct bin_node **BST)
{
    struct bin_node *cur = *BST, *visited = NULL;
    if (cur == NULL) return;
    while (top != -1 || cur != NULL)
    {
        /* when stack is empty and current is NULL, loop done.
        Because when it traverses to the root node,
        stack is empty but cur == (*BST)->right. */
        if (cur != NULL)
        {
            push(cur);
            cur = cur->left;
        }
        else {
            /* judge whether the top element in stack has the right subtree
            and whether the top element was ever used before. */
            if (visited == stack[top]->right || stack[top]->right == NULL)
            {
                visited = popup();
                printf("%" PRId32"\040", visited->node_id);
            }
            /* if it didn't, push the right subtree of the top element into stack. */
            else cur = stack[top]->right;
        }
    }
    return;
}

void delete_all_nodes_in_BST(struct bin_node *node)
{
    if (node == NULL) return;
    delete_all_nodes_in_BST(node->left);
    delete_all_nodes_in_BST(node->right);
    free(node);
    return;
}