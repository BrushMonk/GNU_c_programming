#pragma once
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "UDGraph.c"

struct binomial_node
{   struct tree_node *node;
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

static struct tree_node *extract_min_binomial_node(struct binomial_heap *heap)
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

static void insert_a_node_in_binomial_heap(struct binomial_heap *heap, struct tree_node *const cand)
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
    struct tree_node *decr_node = heap->prior_queue[id]->node;
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

static void insert_adj_multilines_in_binomial_heap(const struct tree_node *node, const struct UDGraph_info *UDGraph, struct binomial_heap *heap, _Bool flag)
{
    /* next adjacent node */
    struct adj_multiline *adj_line = UDGraph->adj[node->node_id];
    while (adj_line != NULL)
    {
        /* candidate inserted into unvisited set */
        struct tree_node *cand;
        *cand = (struct tree_node){-1,
        adj_line->weight + node->dist * flag
        , 0, node->node_id, 0, 0};
        cand->node_id = (adj_line->i_node != node->node_id) ? adj_line->i_node : adj_line->j_node;
        if (decrease_binomial_key(heap, cand->node_id, cand->dist) == NO_NODEID)
            insert_a_node_in_binomial_heap(heap, cand);
        else
        {
            free(cand);
            continue;
        }
        adj_line = (adj_line->i_node == node->node_id) ? adj_line->i_next : adj_line->j_next;
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

#define DIJKSTRA 1
struct tree_node *Dijkstra_algorithm_in_UDGraph(const struct UDGraph_info *UDGraph, int src, int dest)
{
    /* the root of shortest path tree */
    struct tree_node *SPT_root;
    /* put src node into SPT as root */
    *SPT_root = (struct tree_node){src, 0, 0, -1, 0};
    /* loop current node id */
    struct tree_node *cur = SPT_root;
    /* find the minimum-dist node from binomial heap */
    struct binomial_heap unvisited = (struct binomial_heap){0};
    unvisited.head->node = cur;
    struct tree_node *visited[NODE_NUM];
    memset(visited, 0, NODE_NUM * sizeof(struct tree_node *));
    while (cur->node_id != dest || unvisited.head != NULL)
    {
        /* find the minimum-dist node from binomial heap */
        cur = extract_min_binomial_node(&unvisited);
        insert_adj_multilines_in_binomial_heap(cur, UDGraph, &unvisited, DIJKSTRA);
        visited[cur->node_id] = cur;
        if (cur->parent_id != -1)
            insert_leaf_in_tree_node(visited[cur->parent_id], cur);
    }
    delete_all_nodes_in_binomial_heap(unvisited.head);
    memset(unvisited.prior_queue, 0, NODE_NUM * sizeof(struct tree_node *));
    free(&unvisited); free(visited);
    if (cur->node_id != dest)
    {
        delete_all_nodes_in_undirc_tree(SPT_root);
        return SPT_root = NULL;
    }
    /* copy DGraph_node to shortest_path */
    struct tree_node *path_node, *last;
    for (struct tree_node *i = cur; i != NULL; i = i->parent)
    {
        path_node = copy_to_undirc_shortest_list(i);
        if (last != NULL)
        {
            path_node->next[0] = last;
            last->parent = path_node;
        }
        last = path_node;
    }
    delete_all_nodes_in_undirc_tree(SPT_root);
    return path_node;
}

#define PRIM 0
struct tree_node *Prim_algorithm_in_UDGraph(const struct UDGraph_info *UDGraph, int src)
{
    /* the root of minimum spanning tree */
    struct tree_node *MST_root;
    /* put src node into MST as root */
    *MST_root = (struct tree_node){src, 0, 0, -1, 0};
    /* loop current node id */
    struct tree_node *cur = MST_root;
    /* find the minimum-dist node from binomial heap */
    struct binomial_heap unvisited = (struct binomial_heap){0};
    unvisited.head->node = cur;
    struct tree_node *visited[NODE_NUM];
    memset(visited, 0, NODE_NUM * sizeof(struct tree_node *));
    while (unvisited.head != NULL)
    {
        /* find the minimum-dist node from binomial heap */
        cur = extract_min_binomial_node(&unvisited);
        insert_adj_multilines_in_binomial_heap(cur, UDGraph, &unvisited, PRIM);
        visited[cur->node_id] = cur;
        if (cur->parent_id != -1)
            insert_leaf_in_tree_node(visited[cur->parent_id], cur);
    }
    free(&unvisited); free(visited);
    return MST_root;
}

static void merge_sort_undirc_line(struct adj_multiline **restrict arr, size_t len)
{
    struct adj_multiline *sub_arr[len];
    for (size_t e = 0; e < len; e++)
        sub_arr[e] = arr[e];

    for (size_t depth = 1; depth < len; depth <<= 1)
    for (size_t left = 0; left < len; left += depth)
    {
        size_t right = (left + depth < len) ? left + depth - 1 : len - 1;
        size_t middle = (left == right) ? left : 1 + left + ((right - left) >> 1);
        size_t i, j, k;
        for ( i = left, j = middle + 1, k = 0; i <= middle && j <= right; k++)
        arr[k] = (sub_arr[i]->weight < sub_arr[j]->weight) ? sub_arr[i++] : sub_arr[j++];
        while (i <= middle)
        arr[k++] = sub_arr[i++];
        while (j <= right)
        arr[k++] = sub_arr[j++];
    }
    return;
}

static int find_disjt_root(int *disjt_set, int node_id)
{
    return node_id == disjt_set[node_id] ? node_id : (disjt_set[node_id] = find_disjt_root(disjt_set, node_id));
}

/* get the set made up of all UDGraph lines in order from small to great */
static struct adj_multiline **get_lines_set_in_ascd_order(const struct UDGraph_info *UDGraph)
{
    /* a set made up of all UDGraph lines in order from small to great */
    struct adj_multiline **lines_set = (struct adj_multiline **)malloc(UDGraph->line_num * 8UL);
    struct adj_multiline *cur;
    for (size_t v = 0, e = 0; v < NODE_NUM && e < UDGraph->line_num; v++)
    {
        cur = UDGraph->adj[v];
        while (cur != NULL)
        {
            if (cur->ismarked == 0)
            {
                cur->ismarked = 1;
                lines_set[e++] = cur;
            }
            cur = cur->i_node == v ? cur->i_next : cur->j_next;
        }
    }
    merge_sort_undirc_line(lines_set, UDGraph->line_num);
    for (size_t v = 0; v < NODE_NUM; v++)
    {
        cur = UDGraph->adj[v];
        while (cur != NULL)
        {
            if (cur->ismarked == 1)
                cur->ismarked = 0;
            cur = cur->i_node == v ? cur->i_next : cur->j_next;
        }
    }
    return lines_set;
}

struct tree_node *Kruskal_algorithm_in_UDGraph(const struct UDGraph_info *UDGraph)
{
    struct adj_multiline **lines_set = get_lines_set_in_ascd_order(UDGraph);
    /* a set made up of all UDGraph nodes */
    struct tree_node *nodes_set[NODE_NUM];
    /* disjoint set of node id */
    int disjt_set[NODE_NUM];
    for (int v = 0; v < NODE_NUM; v++)
    {
        nodes_set[v] = (struct tree_node *)malloc(sizeof(struct tree_node));
        memset(nodes_set[v], 0, sizeof(struct tree_node));
        nodes_set[v]->node_id = v;
        disjt_set[v] = v;
    }
    struct tree_node *MST_root = nodes_set[lines_set[0]->i_node];
    MST_root->parent_id = -1;
    for (size_t e = 0; e < UDGraph->line_num; e++)
    {
        if (find_disjt_root(disjt_set, lines_set[e]->i_node) != find_disjt_root(disjt_set, lines_set[e]->j_node))
        {
            if (disjt_set[lines_set[e]->j_node] == MST_root->node_id)
            {
                nodes_set[lines_set[e]->i_node]->dist = lines_set[e]->weight;
                nodes_set[lines_set[e]->i_node]->parent_id = lines_set[e]->j_node;
                insert_leaf_in_tree_node(nodes_set[lines_set[e]->j_node], nodes_set[lines_set[e]->i_node]);
            }
            else
            {
                nodes_set[lines_set[e]->j_node]->dist = lines_set[e]->weight;
                nodes_set[lines_set[e]->j_node]->parent_id = lines_set[e]->i_node;
                insert_leaf_in_tree_node(nodes_set[lines_set[e]->i_node], nodes_set[lines_set[e]->j_node]);
            }
            /* merge j_node to the set that i_node is belong to */
            disjt_set[lines_set[e]->j_node] = disjt_set[lines_set[e]->i_node];
        }
    }
    free(lines_set);
    return MST_root;
}