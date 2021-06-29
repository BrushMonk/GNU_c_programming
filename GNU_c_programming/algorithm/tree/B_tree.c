#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <inttypes.h>
/* MAX_DEGREE is the maximum number of children */
#define MAX_DEGREE 128
struct B_node {
    int32_t *key;
    int *fd;
    struct B_node **child;
    int16_t last_index;
    _Bool isleaf;};

static _Atomic(int8_t) top_in_node_stack = -1;
static struct B_node* volatile node_stack[INT8_MAX];
/* pop up out of node_stack */
static struct B_node* popup_node(void)
{
    if (top_in_node_stack == -1)
    {
        perror("pop up on the empty node_stack, abort:");
        exit(-1);
    }
    return node_stack[top_in_node_stack--];
}
/* push struct B_node into node_stack */
static void push_node(struct B_node *node)
{
    if (top_in_node_stack == INT8_MAX)
    {
        perror("node_stack overflow:");
        exit(-1);
    }
    node_stack[++top_in_node_stack] = node;
}

static _Atomic(int8_t) top_in_pos_stack = -1;
static int16_t volatile pos_stack[INT8_MAX];
/* pop up out of pos_stack */
static int16_t popup_pos(void)
{
    if (top_in_pos_stack == -1)
    {
        perror("pop up on the empty node_stack, abort:");
        exit(-1);
    }
    return pos_stack[top_in_pos_stack--];
}
/* push position number into pos_stack */
static void push_pos(int16_t pos)
{
    if (top_in_pos_stack == INT8_MAX)
    {
        perror("pos_stack overflow:");
        exit(-1);
    }
    pos_stack[++top_in_pos_stack] = pos;
}

static int get_string(char **ptr_to_ptr)
{
    size_t i;
    char max_input[BUFSIZ];
    if (fgets(max_input, BUFSIZ, stdin))
    {
        for (i = 0; max_input[i] != '\n' || max_input[i] != '\0' || i < BUFSIZ; i++);
        if (max_input[i] == '\n') max_input[i] = '\0';
        int len = strlen(max_input) + 1;
        if ((*ptr_to_ptr = (char *)realloc(*ptr_to_ptr,
        len * sizeof(char)))==NULL)
        {
        /* because the spaces where pointer inside structure points
        are not allocated and initialized. */
            perror("fail to allocate this input string");
            exit(EXIT_FAILURE);
        }
        strcpy(*ptr_to_ptr, max_input);
        if (**ptr_to_ptr != '\0') return len;
        else return -1;
    }
    else return -1;
}

void inorder_trav_to_B_tree(struct B_node *node)
{
    if (node == NULL) return;
    int16_t i;
    for (i = 0; i <= node->last_index; i++)
    {
        inorder_trav_to_B_tree(node->child[i]);
        printf("%" PRId32"\040", node->key[i]);
    }
    inorder_trav_to_B_tree(node->child[i]);
    return;
}

void preorder_trav_to_B_tree(struct B_node *node)
{
    if (node == NULL) return;
    int16_t i;
    for (i = 0; i <= node->last_index; i++)
    {
        printf("%" PRId32"\040", node->key[i]);
        preorder_trav_to_B_tree(node->child[i]);
    }
    preorder_trav_to_B_tree(node->child[i]);
    return;
}

static int16_t look_up_a_key_pos_in_a_B_node(struct B_node *const node, int32_t unkown_key)
/* use binary search method to position */
{
    int16_t left = 0, right = node->last_index;
    while (left <= right)
    {
        int16_t middle = left + ((right - left) >> 1);
        if (node->key[middle] == unkown_key)
            return middle;
        else if (node->key[middle] > unkown_key)
            right = middle - 1;
        else left = middle + 1;
    }
    return node->key[left - 1] < unkown_key ? left : left - 1;
}

int16_t look_up_a_key_in_B_tree(struct B_node **const B_tree, int32_t unkown_key)
{
    struct B_node *cur = *B_tree;
    int16_t pos = 0;
    while (cur)
    {
        pos = look_up_a_key_pos_in_a_B_node(cur, unkown_key);
        if (cur->key[pos] != unkown_key)
        {
            push_node(cur);
            cur = cur->child[pos];
            push_pos(pos);
            pos = 0;
        }
        else break;
    }
    if (!cur)
    {
        fprintf(stderr, "no key value in B tree!\n");
        top_in_node_stack = -1;
        top_in_pos_stack = -1;
        return -1;
    }
    top_in_node_stack = -1;
    top_in_pos_stack = -1;
    /* return the position of key. */
    return pos;
}

int16_t insert_a_key_in_a_B_node(struct B_node *node, int32_t new_key)
{
    int16_t insert_pos = look_up_a_key_pos_in_a_B_node(node, new_key);
    if (node->key[insert_pos] == new_key)
        return -1;
    for (int16_t i = node->last_index + 1; i > insert_pos; i--)
    {
        node->key[i] = node->key[i - 1];
        node->child[i + 1] = node->child[i];
        node->fd[i] = node->fd[i - 1];
    }
    node->last_index++;
    node->key[insert_pos] = new_key;
    return insert_pos;
}

struct B_node *alloc_a_new_B_node(void)
{
    struct B_node *node = (struct B_node *)malloc(sizeof(struct B_node));
    *node = (struct B_node){0};
    node->key = (int32_t *)malloc(MAX_DEGREE * sizeof(int32_t));
    memset(node->key, -1, MAX_DEGREE * sizeof(int32_t));
    node->child = (struct B_node **)malloc((MAX_DEGREE + 1) * sizeof(struct B_node *));
    memset(node->child, 0, (MAX_DEGREE + 1) * sizeof(struct B_node *));
    node->fd = (int *)malloc(MAX_DEGREE * sizeof(int));
    memset(node->fd, -1, MAX_DEGREE * sizeof(int));
    return node;
}

int write_a_file_in_a_B_node(struct B_node *node, int16_t pos)
{
    char *file_path;
    puts("Please enter the path of a file in B tree:");
    while (get_string(&file_path) < 0)
        puts("input error! Please enter the path of a file in B tree again:");
    node->fd[pos] = open(file_path, O_WRONLY | O_APPEND | O_RDWR | O_SYNC | __O_CLOEXEC);
    if (node->fd[pos] < 0)
    {
       perror("open error");
       return -1;
    }
    printf("Please enter a string that you want to write into"
    "file node->fd[%" PRId16"] %d:\n", pos, node->fd[pos]);
    char *string;
    size_t len = get_string( &string );
    /* pwrite is a atomic function combining write and lseek */
    if (pwrite(node->fd[pos], string, len, BUFSIZ) != len)
        perror("the string pwrite error");
    return node->fd[pos];
}

int close_a_file_in_a_B_node(struct B_node *node, int16_t pos)
{
    int state = close(node->fd[pos]);
    if (state < 0)
    {
       perror("close error");
       return -1;
    }
    node->fd[pos] = -1;
    return 0;
}

int insert_a_key_in_B_tree(struct B_node **B_tree, int32_t new_key)
{
    /* if the B_tree is NULL */
    if (*B_tree == NULL)
    {
        struct B_node *root = alloc_a_new_B_node();
        root->key[0] = new_key;
        root->isleaf = 1;
        return 0;
    }
    /*look up for the position of insertion. */
    struct B_node *cur = *B_tree;
    while ( !cur->isleaf )
    {
        int16_t pos = look_up_a_key_pos_in_a_B_node(cur, new_key);
        if (cur->key[pos] != new_key)
        {
            push_node(cur);
            cur = cur->child[pos];
            pos = 0;
        }
        else
        {
            fprintf(stderr, "insert failed. This B tree has already a key value %" PRId32".\n", new_key);
            return -1;
        }
    }
    /* insert the new_key. */
    if (insert_a_key_in_a_B_node(cur, new_key) < 0)
    {
        fprintf(stderr, "insert failed. This B tree has already a key value %" PRId32".\n", new_key);
        top_in_node_stack = -1; return -1;
    }
    /* split up the ancestor nodes whose elements is over 128. */
    while (cur->last_index == MAX_DEGREE - 1)
    {
        int16_t split_pos = (MAX_DEGREE - 1) >> 1;
        struct B_node *new_node = alloc_a_new_B_node();
        new_node->last_index = MAX_DEGREE - 2 - split_pos;
        for (int16_t i = 0; i <= new_node->last_index; i++)
        {
            new_node->child[i] = cur->child[split_pos + 1 + i];
            new_node->key[i] = cur->key[split_pos + 1 + i];
            new_node->fd[i] = cur->fd[split_pos + i];
            cur->child[split_pos + 1 + i] = NULL;
            cur->fd[split_pos + i] = -1;
            cur->key[split_pos + 1 + i] = 0;
        }
        new_node->child[new_node->last_index + 1] = cur->child[MAX_DEGREE - 1];
        cur->child[MAX_DEGREE - 1] = NULL;
        new_node->isleaf = cur->isleaf;
        cur->last_index = split_pos - 1;
        if (top_in_node_stack == -1)
        {
            /* create a new root after spliting the current root. */
            struct B_node *new_root = alloc_a_new_B_node();
            new_root->key[0] = cur->key[split_pos];
            new_root->child[0] = cur;
            new_root->child[1] = new_node;
            *B_tree = new_root;
            cur->key[split_pos] = 0;
            cur = new_root;
        }
        else
        {
            /* insert middle key value in its ancestor node. */
            int16_t insert_pos = insert_a_key_in_a_B_node(node_stack[top_in_node_stack], cur->key[split_pos]);
            node_stack[top_in_node_stack]->child[insert_pos] = cur;
            node_stack[top_in_node_stack]->child[insert_pos + 1] = new_node;
            cur->key[split_pos] = 0;
            cur = popup_node();
        }
    }
    top_in_node_stack = -1;
    printf("insert key value %" PRId32" successfully.\n", new_key);
    return 0;
}

_Bool file_is_occupied_in_a_B_node(struct B_node *node, int16_t pos)
{
    if (fcntl(node->fd[pos], F_GETFL) != O_NONBLOCK)
    {
        fprintf(stderr, "The file with key %" PRId32" is occupied by some thread.\n", node->key[pos]);
        return 1;
    }
    puts("No occupancy in the node");
    return 0;
}

void free_a_node_in_B_tree(struct B_node *node)
{
    struct B_node *tmp = node;
    free(tmp->key);
    free(tmp->fd);
    free(tmp->child);
    free(tmp);
    return;
}

static void borrow_a_key_from_left_sibling(struct B_node *node,
struct B_node *parent, int16_t pos_in_parent, struct B_node *left_sibling)
{
    insert_a_key_in_a_B_node(node, parent->key[pos_in_parent - 1]);
    node->child[0] = left_sibling->child[left_sibling->last_index + 1];
    /* copy info from end of left_sibling into correct location in parent */
    parent->key[pos_in_parent - 1] = left_sibling->key[left_sibling->last_index];
    parent->fd[pos_in_parent - 1] = left_sibling->fd[left_sibling->last_index];
    left_sibling->child[left_sibling->last_index + 1] = NULL;
    left_sibling->key[left_sibling->last_index] = 0;
    left_sibling->fd[left_sibling->last_index] = -1;
    left_sibling->last_index--;
}

static void borrow_a_key_from_right_sibling(struct B_node *node,
struct B_node *parent, int16_t pos_in_parent, struct B_node *right_sibling)
{
    insert_a_key_in_a_B_node(node, parent->key[pos_in_parent]);
    node->child[node->last_index + 1] = right_sibling->child[0];
    /* copy info from first of right_sibling into correct location in parent */
    parent->key[pos_in_parent] = right_sibling->key[0];
    parent->fd[pos_in_parent] = right_sibling->fd[0];
    for (int16_t i = 0; i <= right_sibling->last_index; i++)
    {
        right_sibling->key[i] = right_sibling->key[i + 1];
        right_sibling->fd[i] = right_sibling->fd[i + 1];
        right_sibling->child[i] = right_sibling->child[i + 1];
    }
    right_sibling->child[node->last_index + 1] = NULL;
    right_sibling->key[node->last_index] = 0;
    right_sibling->fd[node->last_index] = -1;
    right_sibling->last_index--;
}

static struct B_node* merge_left_parent_right_B_nodes(struct B_node *left,
struct B_node *parent, int16_t left_pos_in_parent, struct B_node *right)
{
    left->key[left->last_index + 1] = parent->key[left_pos_in_parent];
    left->fd[left->last_index + 1] = parent->fd[left_pos_in_parent];
    for (int16_t i = 0; i <= left->last_index; i++)
    {
        left->key[left->last_index + 2 + i] = right->key[i];
        left->fd[left->last_index + 2 + i] = right->fd[i];
        left->child[left->last_index + 2 + i] = right->child[i];
    }
    left->last_index += 2 + right->last_index;
    left->child[left->last_index + 1] = right->child[right->last_index + 1];
    for (int16_t i = left_pos_in_parent; i < parent->last_index; i++)
    {
        parent->key[i] = parent->key[i + 1];
        parent->fd[i] = parent->fd[i + 1];
        parent->child[i + 1] = parent->child[i + 2];
    }
    parent->child[parent->last_index + 1] = NULL;
    parent->key[parent->last_index] = 0;
    parent->fd[parent->last_index] = -1;
    parent->last_index--;
    return left;
}

int delete_a_key_in_B_leaf_and_merge(struct B_node *leaf, int16_t del_pos)
{
    /* when the number of key in this leaf is 63. */
    if (leaf->last_index < (MAX_DEGREE - 2) >> 1)
    {
        if (file_is_occupied_in_a_B_node(leaf, del_pos))
            return -1;
        else
        {
            for (int16_t i = del_pos; i < leaf->last_index; i++)
            {
                leaf->key[i] = leaf->key[i + 1];
                leaf->fd[i] = leaf->fd[i + 1];
            }
            leaf->last_index--;
            leaf->key[leaf->last_index] = 0;
            leaf->fd[leaf->last_index] = -1;
        }
        struct B_node *cur = leaf;
        while (top_in_node_stack != -1 && cur->last_index < (MAX_DEGREE - 2) >> 1)
        {
            struct B_node *left_sibling = NULL;
            if (pos_stack[top_in_pos_stack])
                left_sibling = node_stack[top_in_node_stack]->child[pos_stack[top_in_pos_stack] - 1];
            struct B_node *right_sibling = NULL;
            if (pos_stack[top_in_pos_stack] != node_stack[top_in_node_stack]->last_index + 1)
                right_sibling = node_stack[top_in_node_stack]->child[pos_stack[top_in_pos_stack] + 1];
            /* when the number of key in left_sibling is greater than or equal to 64. */
            if (left_sibling && left_sibling->last_index >= (MAX_DEGREE - 2) >> 1)
            {
                borrow_a_key_from_left_sibling(cur, node_stack[top_in_node_stack]
                , pos_stack[top_in_pos_stack], left_sibling);
                break;
            }
            /* when the number of key in right_sibling is greater than or equal to 64. */
            else if (right_sibling && right_sibling->last_index >= (MAX_DEGREE - 2) >> 1)
            {
                borrow_a_key_from_right_sibling(cur, node_stack[top_in_node_stack]
                , pos_stack[top_in_pos_stack], right_sibling);
                break;
            }
            /* when the numbers of key in both left_sibling and right_sibling are 63. */
            else
            {
                if (left_sibling)
                {
                    left_sibling = merge_left_parent_right_B_nodes(left_sibling,
                    node_stack[top_in_node_stack], pos_stack[top_in_pos_stack] - 1, cur);
                    free_a_node_in_B_tree(cur);
                    cur = left_sibling;
                    left_sibling = NULL;
                }
                else
                {
                    cur = merge_left_parent_right_B_nodes(cur, node_stack[top_in_node_stack],
                    pos_stack[top_in_pos_stack], right_sibling);
                    free_a_node_in_B_tree(right_sibling);
                    right_sibling = NULL;
                }
            }
            cur = popup_node();
            del_pos = popup_pos();
        }
        return 0;
    }
    /* when the number of key in this leaf is greater than or equal to 64. */
    else
    {
        if (file_is_occupied_in_a_B_node(leaf, del_pos))
            return -1;
        for (int16_t i = del_pos; i < leaf->last_index; i++)
        {
            leaf->key[i] = leaf->key[i + 1];
            leaf->fd[i] = leaf->fd[i + 1];
        }
        leaf->last_index--;
        leaf->key[leaf->last_index] = 0;
        leaf->fd[leaf->last_index] = -1;
        return 0;
    }
}

int delete_a_key_and_fill_from_left_subtree_in_B_tree(struct B_node **B_tree, int32_t key_to_be_del)
{
    struct B_node *cur = *B_tree;
    int16_t del_pos;
    while (cur)
    {
        del_pos = look_up_a_key_pos_in_a_B_node(cur, key_to_be_del);
        if (cur->key[del_pos] != key_to_be_del)
        {
            push_node(cur);
            cur = cur->child[del_pos];
            push_pos(del_pos);
            del_pos = 0;
        }
        else break;
    }
    if (!cur)
    {
        fprintf(stderr, "no key value in B tree!\n");
        top_in_node_stack = -1;
        top_in_pos_stack = -1;
        return -1;
    }
    #define B_TREE_HAS_ONLY_ONE_KEY_TO_DELETE !((cur != *B_tree) \
    | del_pos | cur->last_index | !cur->isleaf)
    if (B_TREE_HAS_ONLY_ONE_KEY_TO_DELETE)
    {
        *B_tree = NULL;
        free_a_node_in_B_tree(cur);
    }
    else if (cur->isleaf)
    {
        if (delete_a_key_in_B_leaf_and_merge(cur, del_pos))
        {
            fprintf(stderr, "fail to delete key %" PRId32" owe to the file open!\n", cur->key[del_pos]);
            top_in_node_stack = -1;
            top_in_pos_stack = -1;
            return -1;
        }
    }
    else
    {
        push_node(cur);
        push_pos(del_pos);
        struct B_node *max_in_left_subtree = cur->child[del_pos];
        while (!max_in_left_subtree->isleaf)
        {
            push_node(max_in_left_subtree);
            push_pos(max_in_left_subtree->last_index + 1);
            max_in_left_subtree = max_in_left_subtree->child[max_in_left_subtree->last_index + 1];
        }
        int32_t tmp_key = max_in_left_subtree->key[max_in_left_subtree->last_index];
        if (delete_a_key_in_B_leaf_and_merge(max_in_left_subtree,
        max_in_left_subtree->last_index))
        {
            fprintf(stderr, "fail to delete key %" PRId32".\n"
            "Because the file of max_key_in_left_subtree %" PRId32" is open!\n",
            key_to_be_del, max_in_left_subtree->key[max_in_left_subtree->last_index]);
            top_in_node_stack = -1;
            top_in_pos_stack = -1;
            return -1;
        }
        cur->key[del_pos] = tmp_key;
    }
    top_in_node_stack = -1;
    top_in_pos_stack = -1;
    printf("delete key value %" PRId32" successfully.\n", key_to_be_del);
    return 0;
}

int delete_a_key_and_fill_from_right_subtree_in_B_tree(struct B_node **B_tree, int32_t key_to_be_del)
{
    struct B_node *cur = *B_tree;
    int16_t del_pos;
    while (cur)
    {
        del_pos = look_up_a_key_pos_in_a_B_node(cur, key_to_be_del);
        if (cur->key[del_pos] != key_to_be_del)
        {
            push_node(cur);
            cur = cur->child[del_pos];
            push_pos(del_pos);
            del_pos = 0;
        }
        else break;
    }
    if (!cur)
    {
        fprintf(stderr, "no key value in B tree!\n");
        top_in_node_stack = -1;
        top_in_pos_stack = -1;
        return -1;
    }
    if (B_TREE_HAS_ONLY_ONE_KEY_TO_DELETE)
    {
        *B_tree = NULL;
        free_a_node_in_B_tree(cur);
    }
    else if (cur->isleaf)
    {
        if (delete_a_key_in_B_leaf_and_merge(cur, del_pos))
        {
            fprintf(stderr, "fail to delete key %" PRId32" owe to the file open!\n", cur->key[del_pos]);
            top_in_node_stack = -1;
            top_in_pos_stack = -1;
            return -1;
        }
    }
    else
    {
        push_node(cur);
        push_pos(del_pos);
        struct B_node *min_in_right_subtree = cur->child[del_pos + 1];
        while (!min_in_right_subtree->isleaf)
        {
            push_node(min_in_right_subtree);
            push_pos(0);
            min_in_right_subtree = min_in_right_subtree->child[0];
        }
        int32_t tmp_key = min_in_right_subtree->key[0];
        if (delete_a_key_in_B_leaf_and_merge(min_in_right_subtree, 0))
        {
            fprintf(stderr, "fail to delete key %" PRId32".\n"
            "Because the file of min_key_in_right_subtree %" PRId32" is open!\n",
            key_to_be_del, min_in_right_subtree->key[0]);
            top_in_node_stack = -1;
            top_in_pos_stack = -1;
            return -1;
        }
        cur->key[del_pos] = tmp_key;
    }
    top_in_node_stack = -1;
    top_in_pos_stack = -1;
    printf("delete key value %" PRId32" successfully.\n", key_to_be_del);
    return 0;
}

void delete_all_nodes_in_B_tree(struct B_node *node)
{
    if (node == NULL) return;
    int16_t i;
    for (i = 0; i <= node->last_index; i++)
        delete_all_nodes_in_B_tree(node->child[i]);
    delete_all_nodes_in_B_tree(node->child[i]);
    free_a_node_in_B_tree(node);
    return;
}

void postorder_trav_to_B_tree(struct B_node **B_tree)
{
    struct B_node *cur = *B_tree, *visited = NULL;
    int pos = 0;
    if (cur == NULL) return;
    while (top_in_node_stack = -1 || cur != NULL)
    {
        if (cur != NULL)
        {
            push_node(cur);
            push_pos(pos);
            cur = cur->child[pos];
        }
        else
        {
            if (pos > node_stack[top_in_node_stack]->last_index + 1 || node_stack[top_in_node_stack]->child[pos] == NULL)
            {
                visited = popup_node();
                fputs("Here are all node keys: ", stdin);
                for (int16_t i = 0; i <= visited->last_index; i++)
                    printf("%" PRId32"\040", visited->key[i]);
                putchar('\n');
                fputs("Here are all file discriptors: ", stdin);
                for (int16_t i = 0; i <= visited->last_index; i++)
                    printf("%d\040", visited->fd[i]);
                putchar('\n');
            }
            else
            {
                pos++ = popup_pos();
                cur = node_stack[top_in_node_stack]->child[pos];
            }
        }
    }
    return;
}