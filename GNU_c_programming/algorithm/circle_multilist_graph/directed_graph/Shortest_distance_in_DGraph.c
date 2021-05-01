#pragma once
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "adj_list.c"

/* a node in directed graph */
struct DGraph_node
{   int node_id;
    int64_t dist;
    struct DGraph_node **next;
    int parent_id;
    struct DGraph_node *parent;
    size_t child_num;};

struct binomial_node
{   struct DGraph_node *node;
    size_t degree;
    struct binomial_node *left_child;
    struct binomial_node *parent;
    struct binomial_node *sibling;};

struct binomial_heap
{   struct binomial_node *head;
    /* priority queue for positioning to decrease key */
    struct binomial_node *prior_queue[NODE_NUM];};

static struct binomial_node *link_binomial_trees(struct binomial_node *front
, struct binomial_node *rear, struct binomial_node *first)
{
    if (front->node->dist <= rear->node->dist)
    {
        front->sibling = rear->sibling;
        rear->parent = front;
        rear->sibling = front->left_child;
        front->left_child = rear;
        front->degree++;
    }
    else
    {
        if (front == first) rear = first;
        front->parent = rear;
        front->sibling = rear->left_child;
        rear->left_child = front;
        rear->degree++;
        front = rear;
    }
    return front;
}

static struct DGraph_node *extract_min_binomial_node(struct binomial_heap *heap)
{
    struct binomial_node *min = heap->head, *prev_of_min = NULL;
    struct binomial_node *i;
    *i = (struct binomial_node){0};
    for (i->sibling = heap->head; i->sibling != NULL; i = i->sibling)
    if (i->sibling->node->dist < min->node->dist)
    {
        min = i->sibling;
        prev_of_min = i;
    }
    /* delete minimum-key node */
    if (prev_of_min == NULL)
        heap->head = min->sibling;
    else prev_of_min->sibling = min->sibling;
    /* sort binomial trees in the degree of root */
    struct binomial_node *cur1 = heap->head;
    struct binomial_node *cur2 = min->left_child;
    for (i = min->left_child; i != NULL; i = i->sibling)
        i->parent = NULL;
    /* sort binomial trees in the degree of root */
    heap->head = (cur1->degree < cur2->degree) ? cur1 : cur2;
    while (cur1 != NULL && cur2 != NULL)
    {
        if (cur1->degree < cur2->degree)
        {
            i = cur1;
            i = i->sibling;
            cur1 = cur1->sibling;
        }
        else
        {
            i = cur2;
            i = i->sibling;
            cur2 = cur2->sibling;
        }
    }
    while (cur1 != NULL)
    {
        i = cur1;
        i = i->sibling;
        cur1 = cur1->sibling;
    }
    while (cur2 != NULL)
    {
        i = cur2;
        i = i->sibling;
        cur2 = cur2->sibling;
    }
    /* merge binomial trees from head of binomial_heap */
    for (i = heap->head; i->sibling != NULL;)
    {
        struct binomial_node *next_of_i = i->sibling;
        if ((i->degree != next_of_i->degree) ||
        ((next_of_i->sibling != NULL) && (next_of_i->degree == next_of_i->sibling->degree)))
            i = i->sibling;
        else i = link_binomial_trees(i, next_of_i, heap->head);
    }
    heap->prior_queue[min->node->node_id] = NULL;
    return min->node;
}

static void insert_a_node_in_binomial_heap(struct binomial_heap *heap, const struct DGraph_node *cand)
{
    struct binomial_node *new_node = (struct binomial_node *)malloc(sizeof(struct binomial_node));
    memset(new_node, 0, sizeof(struct binomial_node));
    new_node->node = cand;
    /* set new_node as the first binomial tree in binomial_heap */
    new_node->sibling = heap->head;
    heap->head = new_node;
    /* add new node in first of heap and unite binomial trees one by one */
    struct binomial_node *cur, *next_of_cur;
    for (cur = new_node; cur->sibling != NULL;)
    /* worst comlexity of inserting a node in binomial heap is O(log n) */
    {
        next_of_cur = cur->sibling;
        if (next_of_cur->sibling != NULL &&
        next_of_cur->degree == next_of_cur->sibling->degree) cur = cur->sibling;
        else if (cur->degree != next_of_cur->degree) break;
        else cur = link_binomial_trees(cur, next_of_cur, heap->head);
    }
    heap->prior_queue[cand->node_id] = new_node;
    return;
}

#define NO_NODEID -1
#define WRONG_DIST -2
#define DECR_SUCCESS 0
static int decrease_binomial_key(struct binomial_heap *heap, int id, int64_t new_dist)
{
    struct DGraph_node *decr_node = heap->prior_queue[id]->node;
    if (decr_node == NULL)
    {
        fprintf(stderr, "No decreased_id %d in binomial_heap.\n", id);
        return NO_NODEID;
    }
    if (decr_node->dist <= new_dist)
    {
        fprintf(stderr, "decreased_dist %" PRId64" <=  new_dist %" PRId64".\n", decr_node->dist, new_dist);
        return WRONG_DIST;
    }
    decr_node->dist = new_dist;
    for (struct binomial_node *cur = heap->prior_queue[id];
    cur->parent != NULL && cur->node->dist < cur->parent->node->dist;
    cur = cur->parent)
    {
        int64_t tmp_dist = cur->node->dist;
        cur->node->dist = cur->parent->node->dist;
        cur->parent->node->dist = tmp_dist;
        int tmp_id = cur->node->node_id;
        cur->node->node_id = cur->parent->node->node_id;
        cur->parent->node->node_id = tmp_id;
        tmp_id = cur->node->parent_id;
        cur->node->parent_id = cur->parent->node->parent_id;
        cur->parent->node->parent_id = tmp_id;
        heap->prior_queue[cur->node->node_id] = cur;
        heap->prior_queue[cur->parent->node->node_id] = cur->parent;
    }
    return DECR_SUCCESS;
}

static void insert_adj_nodes_in_binomial_heap(const struct DGraph_node *node, const struct DGraph_info *DGraph, struct binomial_heap *heap, _Bool flag)
{
    /* next adjacent node */
    struct adj_node *next_adj;
    for (next_adj = DGraph->closest_outadj[node->node_id]; next_adj != NULL; next_adj = next_adj->next)
    {
        /* candidate inserted into unvisited set */
        struct DGraph_node *cand = (struct DGraph_node *)malloc(sizeof(struct DGraph_node));
        memset(cand, 0, sizeof(struct DGraph_node));
        cand->node_id = next_adj->node_id;
        cand->parent_id = node->node_id;
        cand->dist = next_adj->weight + flag * node->dist;
        if (decrease_binomial_key(heap, cand->node_id, cand->dist) == NO_NODEID)
            insert_a_node_in_binomial_heap(heap, cand);
        else
        {
            free(cand);
            continue;
        }
    }
    return;
}

static void delete_all_nodes_in_binomial_heap(struct binomial_node *node)
{
    if (node == NULL) return;
    delete_all_nodes_in_binomial_heap(node->left_child);
    delete_all_nodes_in_binomial_heap(node->sibling);
    free(node);
    return;
}

static size_t insert_leaf_in_DGraph_node(struct DGraph_node *node, struct DGraph_node *new_leaf)
{
    size_t middle, left = 0, right = node->child_num - 1;
    while (left <= right)
    {
        middle = left + ((right - left) >> 1);
        if (node->next[middle]->dist == new_leaf->dist)
            break;
        else if (node->next[middle]->dist > new_leaf->dist)
            right = middle - 1;
        else left = middle + 1;
    }
    size_t pos = left > middle ? left : middle;
    if ((node->next = (struct DGraph_node **)realloc(node->next, (node->child_num + 1) * 8UL)) == NULL)
    {
        perror("fail to allocate array");
        exit(EXIT_FAILURE);
    }
    for (size_t i = pos; i < node->child_num; i++)
        node->next[i + 1] = node->next[i];
    node->next[pos] = new_leaf;
    new_leaf->parent = node;
    node->child_num++;
    return pos;
}

static void delete_all_nodes_in_DGraph_tree(struct DGraph_node *node)
{
    if (node->child_num == 0)
    {
        free(node); return;
    }
    for (size_t i = 0; i < node->child_num; i++)
        delete_all_nodes_in_DGraph_tree(node->next[i]);
    free(node);
    return;
}

static struct DGraph_node *copy_DGraph_node_to_shortest_list(struct DGraph_node *node)
{
    struct DGraph_node *list_node = (struct DGraph_node *)malloc(sizeof(struct DGraph_node));
    memset(list_node, 0, sizeof(struct DGraph_node));
    list_node->next = (struct DGraph_node **)malloc(8UL);
    list_node->child_num = 1;
    list_node->dist = node->dist;
    list_node->node_id = node->node_id;
    list_node->parent_id = node->parent_id;
    return list_node;
}

#define DIJKSTRA 1
struct DGraph_node *Dijkstra_algorithm_in_DGraph(const struct DGraph_info *DGraph, int src, int dest)
{
    /* the root of shortest path tree */
    struct DGraph_node *SPT_root;
    *SPT_root = (struct DGraph_node){0};
    /* put src node into SPT as root */
    SPT_root->node_id = src;
    SPT_root->parent_id = -1;
    /* loop current node id */
    struct DGraph_node *cur = SPT_root;
    /* find the minimum-dist node from binomial heap */
    struct binomial_heap unvisited = (struct binomial_heap){0};
    unvisited.head->node = cur;
    struct DGraph_node *visited[NODE_NUM];
    memset(visited, 0, NODE_NUM * sizeof(struct DGraph_node *));
    while (cur->node_id != dest || unvisited.head != NULL)
    {
        /* find the minimum-dist node from binomial heap */
        cur = extract_min_binomial_node(&unvisited);
        insert_adj_nodes_in_binomial_heap(cur, DGraph, &unvisited, DIJKSTRA);
        visited[cur->node_id] = cur;
        if (cur->parent_id != -1)
            insert_leaf_in_DGraph_node(visited[cur->parent_id], cur);
    }
    delete_all_nodes_in_binomial_heap(unvisited.head);
    memset(unvisited.prior_queue, 0, NODE_NUM * sizeof(struct DGraph_node *));
    free(&unvisited); free(visited);
    if (cur->node_id != dest)
    {
        delete_all_nodes_in_DGraph_tree(SPT_root);
        return SPT_root = NULL;
    }
    /* copy DGraph_node to shortest_list */
    struct DGraph_node *list_node, *last;
    for (struct DGraph_node *i = cur; i != NULL; i = i->parent)
    {
        list_node = copy_DGraph_node_to_shortest_list(i);
        if (last != NULL)
        {
            list_node->next[0] = last;
            last->parent = list_node;
        }
        last = list_node;
    }
    delete_all_nodes_in_DGraph_tree(SPT_root);
    return list_node;
}

#define PRIM 0
struct DGraph_node *Prim_algorithm_in_DGraph(const struct DGraph_info *DGraph, int src)
{
    /* the root of minimum spanning tree */
    struct DGraph_node *MST_root;
    *MST_root = (struct DGraph_node){0};
    /* put src node into SPT as root */
    MST_root->node_id = src;
    MST_root->parent_id = -1;
    /* loop current node id */
    struct DGraph_node *cur = MST_root;
    /* find the minimum-dist node from binomial heap */
    struct binomial_heap unvisited = (struct binomial_heap){0};
    unvisited.head->node = cur;
    struct DGraph_node *visited[NODE_NUM];
    memset(visited, 0, NODE_NUM * sizeof(struct DGraph_node *));
    while (unvisited.head != NULL)
    {
        /* find the minimum-dist node from binomial heap */
        cur = extract_min_binomial_node(&unvisited);
        insert_adj_nodes_in_binomial_heap(cur, DGraph, &unvisited, PRIM);
        visited[cur->node_id] = cur;
        if (cur->parent_id != -1)
            insert_leaf_in_DGraph_node(visited[cur->parent_id], cur);
    }
    free(&unvisited); free(visited);
    return MST_root;
}