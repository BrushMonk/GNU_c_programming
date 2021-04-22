#pragma once
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "adj_multilist.c"

/* a node in undirected graph */
struct UDGraph_node
{   int node_id;
    int64_t dist;
    struct UDGraph_node **next;
    int parent_id;
    struct UDGraph_node *parent;
    size_t child_num;};

struct binomial_node
{   struct UDGraph_node *node;
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


static struct UDGraph_node *extract_min_binomial_node(struct binomial_heap *heap)
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

static void insert_a_node_in_binomial_heap(struct binomial_heap *heap, const struct UDGraph_node *cand)
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
    struct UDGraph_node *decr_node = heap->prior_queue[id]->node;
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

static void insert_adj_multinodes_in_binomial_heap(const struct UDGraph_node *node, const struct UDGraph_info *UDGraph, struct binomial_heap *heap, _Bool flag)
{
    /* next adjacent node */
    struct adj_multinode *next_adj = UDGraph->closest_adj[node->node_id];
    while (next_adj != NULL)
    {
        /* candidate inserted into unvisited set */
        struct UDGraph_node *cand = (struct UDGraph_node *)malloc(sizeof(struct UDGraph_node));
        memset(cand, 0, sizeof(struct UDGraph_node));
        cand->node_id = (next_adj->i_node != node->node_id) ? next_adj->i_node : next_adj->j_node;
        cand->parent_id = node->node_id;
        cand->dist = next_adj->weight + flag * node->dist;
        if (decrease_binomial_key(heap, cand->node_id, cand->dist) == NO_NODEID)
            insert_a_node_in_binomial_heap(heap, cand);
        else
        {
            free(cand);
            continue;
        }
        next_adj = (next_adj->i_node == node->node_id) ? next_adj->i_next : next_adj->j_next;
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

static size_t insert_leaf_in_UDGraph_node(struct UDGraph_node *node, struct UDGraph_node *new_leaf)
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
    if ((node->next = (struct UDGraph_node **)realloc(node->next, (node->child_num + 1) * 8UL)) == NULL)
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

static void delete_all_nodes_in_UDGraph_tree(struct UDGraph_node *node)
{
    if (node->child_num == 0)
    {
        free(node); return;
    }
    for (size_t i = 0; i < node->child_num; i++)
        delete_all_nodes_in_UDGraph_tree(node->next[i]);
    free(node);
    return;
}

static struct UDGraph_node *copy_UDGraph_node_to_shortest_list(struct UDGraph_node *node)
{
    struct UDGraph_node *list_node = (struct UDGraph_node *)malloc(sizeof(struct UDGraph_node));
    memset(list_node, 0, sizeof(struct UDGraph_node));
    list_node->next = (struct UDGraph_node **)malloc(8UL);
    list_node->child_num = 1;
    list_node->dist = node->dist;
    list_node->node_id = node->node_id;
    list_node->parent_id = node->parent_id;
    return list_node;
}

#define DIJKSTRA 1
struct UDGraph_node *Dijkstra_algorithm_in_UDGraph(const struct UDGraph_info *UDGraph, int src, int dest)
{
    /* the root of shortest path tree */
    struct UDGraph_node *SPT_root;
    *SPT_root = (struct UDGraph_node){0};
    /* put src node into SPT as root */
    SPT_root->node_id = src;
    SPT_root->parent_id = -1;
    /* loop current node id */
    struct UDGraph_node *cur = SPT_root;
    /* find the minimum-dist node from binomial heap */
    struct binomial_heap unvisited = (struct binomial_heap){0};
    unvisited.head->node = cur;
    struct UDGraph_node *visited[NODE_NUM];
    memset(visited, 0, NODE_NUM * sizeof(struct UDGraph_node *));
    while (cur->node_id != dest || unvisited.head != NULL)
    {
        /* find the minimum-dist node from binomial heap */
        cur = extract_min_binomial_node(&unvisited);
        insert_adj_multinodes_in_binomial_heap(cur, UDGraph, &unvisited, DIJKSTRA);
        visited[cur->node_id] = cur;
        if (cur->parent_id != -1)
            insert_leaf_in_UDGraph_node(visited[cur->parent_id], cur);
    }
    delete_all_nodes_in_binomial_heap(unvisited.head);
    memset(unvisited.prior_queue, 0, NODE_NUM * sizeof(struct UDGraph_node *));
    free(&unvisited); free(visited);
    if (cur->node_id != dest)
    {
        delete_all_nodes_in_UDGraph_tree(SPT_root);
        return SPT_root = NULL;
    }
    /* copy DGraph_node to shortest_list */
    struct UDGraph_node *list_node, *last;
    for (struct UDGraph_node *i = cur; i != NULL; i = i->parent)
    {
        list_node = copy_UDGraph_node_to_shortest_list(i);
        if (last != NULL)
        {
            list_node->next[0] = last;
            last->parent = list_node;
        }
        last = list_node;
    }
    delete_all_nodes_in_UDGraph_tree(SPT_root);
    return list_node;
}

#define PRIM 0
struct UDGraph_node *Prim_algorithm_in_UDGraph(const struct UDGraph_info *UDGraph, int src)
{
    /* the root of minimum spanning tree */
    struct UDGraph_node *MST_root;
    *MST_root = (struct UDGraph_node){0};
    /* put src node into SPT as root */
    MST_root->node_id = src;
    MST_root->parent_id = -1;
    /* loop current node id */
    struct UDGraph_node *cur = MST_root;
    /* find the minimum-dist node from binomial heap */
    struct binomial_heap unvisited = (struct binomial_heap){0};
    unvisited.head->node = cur;
    struct UDGraph_node *visited[NODE_NUM];
    memset(visited, 0, NODE_NUM * sizeof(struct UDGraph_node *));
    while (unvisited.head != NULL)
    {
        /* find the minimum-dist node from binomial heap */
        cur = extract_min_binomial_node(&unvisited);
        insert_adj_multinodes_in_binomial_heap(cur, UDGraph, &unvisited, PRIM);
        visited[cur->node_id] = cur;
        if (cur->parent_id != -1)
            insert_leaf_in_UDGraph_node(visited[cur->parent_id], cur);
    }
    free(&unvisited); free(visited);
    return MST_root;
}

static void heap_sort(struct adj_multinode **restrict arr, int len)
{
    /* initialize i as the last nonleaf node in tree */
    for (int i = len >> 1 ; i >= 0; i--)
    /* the complexity of this procedure is O(n) */
    {
        int min_child = (i << 1) + 1;
        if (min_child + 1 < len && arr[min_child]->weight < arr[min_child + 1]->weight)
            min_child++;
        if (arr[i]->weight >= arr[min_child]->weight) continue;
        else
        {
            struct adj_multinode *tmp = arr[i];
            arr[i] = arr[min_child];
            arr[min_child] = tmp;
        }
    }
    for (int i = len - 1; i > 0; i--)
    /* the complexity of this procedure is O(nlog n) */
    {
        struct adj_multinode *tmp = arr[i];
        arr[i] = arr[0];
        arr[0] = tmp;
        for (int cur = i, max_child = (cur << 1) + 1; max_child < len;)
        /* heapify from this first element. The complexity of this procedure is O(log n). */
        {
            if (max_child + 1 < len && arr[max_child]->weight < arr[max_child + 1]->weight)
                max_child++;
            if (arr[cur]->weight >= arr[max_child]->weight) break;
            else
            {
                struct adj_multinode *tmp = arr[cur]; arr[cur] = arr[max_child]; arr[max_child] = tmp;
                cur = max_child;
                max_child = 2 * cur + 1;
            }
        }
    }
    return;
}

int find_disjt_ele(int *disjt_set, struct UDGraph_node *node)
{
    if (disjt_set[node->node_id] == node->node_id)
        return node->node_id;
    else
    {
        disjt_set[node->node_id] = find_disjt_ele(disjt_set, node);
        return disjt_set[node->node_id];
    }
}

void merge_disjt_ele(int *disjt_set, struct UDGraph_node *node1, struct UDGraph_node *node2)
{
    disjt_set[find_disjt_ele(disjt_set, node1)] = find_disjt_ele(disjt_set, node2);
    return;
}

struct UDGraph_node *Kruskal_algorithm_in_UDGraph(const struct UDGraph_info *UDGraph)
{
    /* a set made up of all UDGraph sides in order from small to great */
    struct adj_multinode *sides_set[UDGraph->side_num];
    struct adj_multinode *cur;
    for (size_t v = 0, e = 0; v < NODE_NUM && e < UDGraph->side_num; v++)
    {
        cur = UDGraph->closest_adj[v];
        while (cur != NULL)
        {
            if (cur->ismarked == 0)
            {
                cur->ismarked = 1;
                sides_set[e++] = cur;
            }
            cur = cur->i_node == v ? cur->i_next : cur->j_next;
        }
    }
    heap_sort(sides_set, (int)UDGraph->side_num);
    /* a set made up of all UDGraph nodes */
    struct UDGraph_node *nodes_set[NODE_NUM];
    /* disjoint set of node id */
    int disjt_set[NODE_NUM];
    for (int v = 0; v < NODE_NUM; v++)
    {
        nodes_set[v] = (struct UDGraph_node *)malloc(sizeof(struct UDGraph_node));
        memset(nodes_set[v], 0, sizeof(struct UDGraph_node));
        nodes_set[v]->node_id = v;
        disjt_set[v] = v;
    }
    struct UDGraph_node *MST_root;
    for (size_t e = 0; e < UDGraph->side_num; e++)
    {
        if (find_disjt_ele(disjt_set, nodes_set[sides_set[e]->i_node]) == find_disjt_ele(disjt_set, nodes_set[sides_set[e]->j_node]))
            continue;
        else
        {
            if (e == 0)
            {
                MST_root = nodes_set[sides_set[e]->i_node];
                MST_root->parent_id = -1;
            }
            if (find_disjt_ele(disjt_set, nodes_set[sides_set[e]->j_node]) == MST_root->node_id)
            {
                nodes_set[sides_set[e]->i_node]->dist = sides_set[e]->weight;
                nodes_set[sides_set[e]->i_node]->parent_id = sides_set[e]->j_node;
                insert_leaf_in_UDGraph_node(nodes_set[sides_set[e]->j_node], nodes_set[sides_set[e]->i_node]);
            }
            else
            {
                nodes_set[sides_set[e]->j_node]->dist = sides_set[e]->weight;
                nodes_set[sides_set[e]->j_node]->parent_id = sides_set[e]->i_node;
                insert_leaf_in_UDGraph_node(nodes_set[sides_set[e]->i_node], nodes_set[sides_set[e]->j_node]);
            }
            merge_disjt_ele(disjt_set, nodes_set[sides_set[e]->i_node], nodes_set[sides_set[e]->j_node]);
        }
    }
    return MST_root;
}