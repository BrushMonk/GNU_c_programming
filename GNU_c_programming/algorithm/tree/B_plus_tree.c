#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <fcntl.h>
#include <string.h>
#define MAX_KEY_NUMBER 255
struct B_plus_node {
    int16_t *key;
    struct B_plus_node **child;
    int16_t last_index;
    int16_t pos_in_parent_node;
    _Bool isleaf;
    int *fd;
    struct B_plus_node *parent;
    struct B_plus_node *sibling;};

static int get_string(char **ptr_to_ptr)
{
    size_t i;
    char max_input[BUFSIZ];
    if (fgets(max_input, BUFSIZ, stdin))
    {
        for ( i = 0; max_input[i] != '\n' || max_input[i] != '\0' || i < BUFSIZ; i++);
        if (max_input[i] == '\n') max_input[i] = '\0';
        int len = strlen(max_input) + 1;
        if ( (*ptr_to_ptr = (char *)realloc(*ptr_to_ptr,
        len * sizeof(char))) == NULL )
        {
        /* because the spaces where pointers inside structure point
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

void inorder_trav_to_B_plus_tree(struct B_plus_node *node)
{
    if (node == NULL) return;
    for (int16_t i = 0; i <= node->last_index; i++)
    {
        inorder_trav_to_B_plus_tree(node->child[i]);
        printf("%" PRId16"\040", node->key[i]);
    }
    return;
}

void preorder_trav_to_B_plus_tree(struct B_plus_node *node)
{
    if (node == NULL) return;
    for (int16_t i = 0; i <= node->last_index; i++)
    {
        printf("%" PRId16"\040", node->key[i]);
        preorder_trav_to_B_plus_tree(node->child[i]);
    }
    return;
}

static int16_t look_up_a_key_pos_in_a_B_plus_node(struct B_plus_node *const node, int16_t unkown_key)
/* use binary search method to position */
{
    int16_t left = 0, right = node->last_index;
    while(left <= right)
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

int16_t look_up_a_leaf_key_in_B_plus_tree(struct B_plus_node **const B_plus_tree, int16_t leaf_key)
{
    struct B_plus_node *cur = *B_plus_tree;
    int16_t pos = 0;
    while (cur)
    {
        pos = look_up_a_key_pos_in_a_B_plus_node(cur, leaf_key);
        if ((cur->key[pos] == leaf_key) & cur->isleaf)
            break;
        else
        {
            cur = cur->child[pos];
            pos = 0;
        }
    }
    if (!cur)
    {
        fprintf(stderr, "no key value in B plus tree!\n");
        return -1;
    }
    /* return the position of key. */
    return pos;
}

struct B_plus_node *alloc_a_new_B_plus_node(_Bool neighbour_isleaf)
{
    struct B_plus_node *node = (struct B_plus_node *)malloc(sizeof(struct B_plus_node));
    *node = (struct B_plus_node){0};
    node->key = (int16_t *)malloc((MAX_KEY_NUMBER + 1) * sizeof(int16_t));
    memset(node->key, -1, MAX_KEY_NUMBER * sizeof(int16_t));
    node->child = (struct B_plus_node **)malloc((MAX_KEY_NUMBER + 1) * sizeof(struct B_plus_node *));
    if (neighbour_isleaf)
    {
        node->isleaf = 1;
        node->fd = (int *)malloc(MAX_KEY_NUMBER * sizeof(int));
        memset(node->fd, -1, MAX_KEY_NUMBER * sizeof(int));
    }
    node->pos_in_parent_node = -1;
    return node;
}

int write_a_file_in_a_B_plus_node(struct B_plus_node *node, int16_t pos)
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
    size_t len = get_string(&string);
    /* pwrite is a atomic function combining write and lseek */
    if (pwrite(node->fd[pos], string, len, BUFSIZ) != len)
        perror("the string pwrite error");
    return node->fd[pos];
}

int16_t insert_a_key_in_a_B_plus_node(struct B_plus_node *node, int16_t new_key)
{
    int16_t insert_pos = look_up_a_key_pos_in_a_B_plus_node(node, new_key);
    if (node->key[insert_pos] == new_key)
        return -1;
    for (int16_t i = node->last_index + 1; i > insert_pos; i--)
    {
        node->key[i] = node->key[i - 1];
        node->child[i] = node->child[i - 1];
    }
    if (node->isleaf)
        for (int16_t i = node->last_index + 1; i > insert_pos; i--)
            node->fd[i] = node->fd[i - 1];
    node->last_index++;
    node->key[insert_pos] = new_key;
    node->fd[insert_pos] = -1;
    if (insert_pos == 0)
    {
        /* if new_key is the minimum, ascend to change the ancestors until root. */
        struct B_plus_node *ancestor = node;
        int16_t ascend_pos = insert_pos;
        while (ancestor && !ascend_pos)
        {
            if (ascend_pos == 0)
                ancestor->parent->key[ancestor->pos_in_parent_node] = new_key;
            ascend_pos = ancestor->pos_in_parent_node;
            ancestor = ancestor->parent;
        }
    }
    return insert_pos;
}

int insert_a_key_in_B_plus_tree(struct B_plus_node **B_plus_tree, int16_t new_key)
{
    /* if the B_tree is NULL */
    if (*B_plus_tree == NULL)
    {
        struct B_plus_node *root = alloc_a_new_B_plus_node(1);
        root->key[0] = new_key;
        return 0;
    }
    /* look up for the position of insertion. */
    struct B_plus_node *cur = *B_plus_tree;
    while (!cur->isleaf)
    {
        int16_t pos = look_up_a_key_pos_in_a_B_plus_node(cur, new_key);
        if (cur->key[pos] != new_key)
        {
            cur = cur->child[pos];
            pos = 0;
        }
        else
        {
            fprintf(stderr, "insert failed. This B plus tree has already a key value %" PRId16".\n", new_key);
            return -1;
        }
    }
    /* insert the new_key. */
    if (insert_a_key_in_a_B_plus_node(cur, new_key) < 0)
    {
        fprintf(stderr, "insert failed. This B plus tree has already a key value %" PRId16".\n", new_key);
        return 0;
    }
    /* split up the ancestor nodes whose elements is over 255. */
    while (cur->last_index == MAX_KEY_NUMBER)
    {
        int16_t split_pos = (MAX_KEY_NUMBER) >> 1;
        struct B_plus_node *new_node = alloc_a_new_B_plus_node(cur->isleaf);
        new_node->last_index = MAX_KEY_NUMBER - split_pos;
        for (int16_t i = 0; i <= new_node->last_index; i++)
        {
            new_node->child[i] = cur->child[split_pos + i];
            new_node->key[i] = cur->key[split_pos + i];
            cur->child[split_pos + i] = NULL;
            cur->key[split_pos + i] = 0;
        }
        new_node->child[new_node->last_index + 1] = cur->child[MAX_KEY_NUMBER];
        cur->child[MAX_KEY_NUMBER] = NULL;
        cur->last_index = split_pos - 1;
        if (new_node->isleaf)
        {
            for (int16_t i = 0; i <= new_node->last_index; i++)
            {
                new_node->fd[i] = cur->fd[split_pos + i];
                cur->fd[split_pos+i] = -1;
            }
            cur->sibling = new_node;
        }
        if (!cur->parent)
        {
            /* create a new root after spliting the cur root. */
            struct B_plus_node *new_root = alloc_a_new_B_plus_node(0);
            new_root->key[0] = cur->key[0];
            new_root->key[1] = new_node->key[0];
            new_root->child[0] = cur;
            new_root->child[1] = new_node;
            cur->parent = new_node->parent = new_root;
            cur->pos_in_parent_node = 0;
            new_node->pos_in_parent_node = 1;
            *B_plus_tree = new_root;
            cur = new_root;
        }
        else {
            /* insert middle key value in its ancestor node. */
            new_node->pos_in_parent_node = insert_a_key_in_a_B_plus_node(cur->parent, cur->key[split_pos]);
            cur->pos_in_parent_node = new_node->pos_in_parent_node - 1;
            new_node->parent = cur->parent;
            cur->parent->child[cur->pos_in_parent_node] = cur;
            new_node->parent->child[new_node->pos_in_parent_node] = new_node;
            cur = cur->parent;
        }
    }
    printf("insert key value %" PRId16" successfully.\n", new_key);
    return 1;
}

_Bool file_is_occupied_in_a_B_plus_leaf(struct B_plus_node *leaf, int16_t pos)
{
    if (fcntl(leaf->fd[pos], F_GETFL, 0) != O_NONBLOCK)
    {
        fprintf(stderr, "The file with key %" PRId16" is occupied by some thread.\n", leaf->key[pos]);
        return 1;
    }
    puts("No occupancy in the leaf");
    return 0;
}

void free_a_node_in_B_plus_tree(struct B_plus_node *node)
{
    struct B_plus_node *tmp = node;
    free(tmp->key);
    free(tmp->child);
    if (tmp->isleaf) free(tmp->fd);
    free(tmp);
    return;
}

static struct B_plus_node* merge_left_parent_right_B_plus_nodes(struct B_plus_node *left,
struct B_plus_node *parent, struct B_plus_node *right)
{
    for (int16_t i = 0; i <= left->last_index; i++)
    {
        left->key[left->last_index + 1 + i] = right->key[i];
        left->fd[left->last_index + 1 + i] = right->fd[i];
        left->child[left->last_index + 1 + i] = right->child[i];
    }
    left->last_index += 1 + right->last_index;
    for (int16_t i = right->pos_in_parent_node; i <= parent->last_index; i++)
    {
        parent->key[i] = parent->key[i + 1];
        parent->fd[i] = parent->fd[i + 1];
        parent->child[i] = parent->child[i + 1];
    }
    parent->last_index--;
    return left;
}

int delete_a_key_in_B_plus_leaf_and_merge(struct B_plus_node *leaf, int16_t del_pos)
{
    /* once the number of key in this leaf is smaller than 127. */
    if ( leaf->last_index < (MAX_KEY_NUMBER >> 1) - 1 )
    {
        if (file_is_occupied_in_a_B_plus_leaf(leaf, del_pos))
            return -1;
        else
        {
            for (int16_t i = del_pos; i < leaf->last_index; i++)
            {
                leaf->key[i] = leaf->key[i + 1];
                leaf->fd[i] = leaf->fd[i + 1];
            }
            leaf->last_index--;
        }
        struct B_plus_node *cur = leaf;
        while (cur && cur->last_index < (MAX_KEY_NUMBER >> 1) - 1)
        {
            struct B_plus_node *left_sibling = NULL;
            if (cur->pos_in_parent_node)
                left_sibling = cur->parent->child[cur->pos_in_parent_node - 1];
            struct B_plus_node *right_sibling = NULL;
            if (cur->pos_in_parent_node != cur->parent->last_index + 1)
                right_sibling = cur->parent->child[cur->pos_in_parent_node + 1];
            /* when the number of key in left_sibling is greater than or equal to 64. */
            if (left_sibling && left_sibling->last_index >= (MAX_KEY_NUMBER >> 1) - 1)
            {
                insert_a_key_in_a_B_plus_node(cur,
                cur->parent->key[left_sibling->pos_in_parent_node]);
                cur->child[0] = left_sibling->child[left_sibling->last_index];
                cur->fd[0] = left_sibling->fd[left_sibling->last_index];
                left_sibling->child[left_sibling->last_index] = NULL;
                left_sibling->key[left_sibling->last_index] = 0;
                left_sibling->fd[left_sibling->last_index] = -1;
                left_sibling->last_index--;
                break;
            }
            /* when the number of key in right_sibling is greater than or equal to 64. */
            else if (right_sibling && right_sibling->last_index >= (MAX_KEY_NUMBER - 2) >> 1)
            {
                insert_a_key_in_a_B_plus_node(cur,
                cur->parent->key[right_sibling->pos_in_parent_node]);
                cur->child[cur->last_index] = right_sibling->child[0];
                cur->fd[cur->last_index] = right_sibling->fd[0];
                for (int16_t i = 0; i <= right_sibling->last_index; i++)
                {
                    right_sibling->key[i] = right_sibling->key[i + 1];
                    right_sibling->fd[i] = right_sibling->fd[i + 1];
                    right_sibling->child[i] = right_sibling->child[i + 1];
                }
                right_sibling->last_index--;
                /* if minimal key of right_sibling changed, ascend to change the ancestors until root. */
                for (struct B_plus_node *ancestor = cur;
                ancestor && ancestor->pos_in_parent_node;
                ancestor = ancestor->parent)
                    ancestor->parent->key[ancestor->pos_in_parent_node] = right_sibling->key[0];
                break;
            }
            /* when the numbers of key in both left_sibling and right_sibling are 63. */
            else
            {
                if (left_sibling)
                {
                    left_sibling = merge_left_parent_right_B_plus_nodes(left_sibling,
                    cur->parent, cur);
                    if (cur->isleaf)
                        left_sibling->sibling = cur->sibling;
                    free_a_node_in_B_plus_tree(cur);
                    cur = left_sibling;
                    left_sibling = NULL;
                }
                else
                {
                    cur = merge_left_parent_right_B_plus_nodes(cur,
                    cur->parent, right_sibling);
                    if (cur->isleaf)
                        cur->sibling = right_sibling->sibling;
                    free_a_node_in_B_plus_tree(right_sibling);
                    right_sibling = NULL;
                }
            }
            cur = cur->parent;
        }
        return 0;
    }
    /* when the number of key in this leaf is greater than or equal to 64. */
    else
    {
        if (file_is_occupied_in_a_B_plus_leaf(leaf, del_pos))
            return -1;
        for (int16_t i = del_pos; i < leaf->last_index; i++)
        {
            leaf->key[i] = leaf->key[i + 1];
            leaf->fd[i] = leaf->fd[i + 1];
        }
        leaf->last_index--;
        return 0;
    }
}

int delete_a_key_and_fill_from_min_key_in_B_plus_tree(struct B_plus_node **B_plus_tree, int16_t key_to_be_del)
{
    struct B_plus_node *cur = *B_plus_tree;
    int16_t del_pos;
    while (cur)
    {
        del_pos = look_up_a_key_pos_in_a_B_plus_node(cur, key_to_be_del);
        if ((cur->key[del_pos] == key_to_be_del) & cur->isleaf)
            break;
        else
        {
            cur = cur->child[del_pos];
            del_pos = 0;
        }
    }
    if (!cur)
    {
        fprintf(stderr, "No key value in B plus tree!\n");
        return -1;
    }
    #define B_PLUS_TREE_HAS_ONLY_ONE_KEY_TO_DELETE !((cur != *B_plus_tree) \
    | del_pos | cur->last_index | !cur->isleaf)
    if (B_PLUS_TREE_HAS_ONLY_ONE_KEY_TO_DELETE)
    {
        *B_plus_tree = NULL;
        free_a_node_in_B_plus_tree(cur);
    }
    else if (cur->isleaf)
    {
        if (!delete_a_key_in_B_plus_leaf_and_merge(cur, del_pos))
        {
            fprintf(stderr, "fail to delete key %" PRId16" owe to the file open!\n", key_to_be_del);
            return -1;
        }
    }
    else
    {
        struct B_plus_node *leaf_of_cur = cur->child[del_pos];
        while (!leaf_of_cur->isleaf)
            leaf_of_cur = leaf_of_cur->child[0];
        if (delete_a_key_in_B_plus_leaf_and_merge(leaf_of_cur, 0))
        {
            fprintf(stderr, "Fail to delete key %" PRId16".\n"
            "Because the file of min_key_in_leaf %" PRId16" is open!\n",
            key_to_be_del, leaf_of_cur->key[0]);
            return -1;
        }
        /* ascend to change the ancestors until the ancestor is not equal to key. */
        int16_t min_key_in_leaf = leaf_of_cur->key[0];
        while (leaf_of_cur->parent->key[0] == key_to_be_del)
        {
            leaf_of_cur->parent->key[0] = min_key_in_leaf;
            leaf_of_cur = leaf_of_cur->parent;
        }
    }
    printf("delete key value %" PRId16" successfully.\n", key_to_be_del);
    return 0;
}

void delete_all_nodes_in_B_plus_tree(struct B_plus_node *node)
{
    if (node == NULL) return;
    int16_t i;
    for (i = 0; i <= node->last_index; i++)
        delete_all_nodes_in_B_plus_tree(node->child[i]);
    free_a_node_in_B_plus_tree(node);
    return;
}