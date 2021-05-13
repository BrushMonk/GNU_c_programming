#pragma once
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define NODE_NUM 1024
/* adjacency list node */
struct adj_node
{   int node_id;
    struct adj_node *next;
    int64_t weight;};

/* weighted directed graph infomation */
struct DGraph_info
{
    /* the closest outdegree adjacency node */
    struct adj_node *outadj[NODE_NUM];
    /* the closest indegree adjacency node */
    struct adj_node *inadj[NODE_NUM];
    size_t line_num;
};

struct dirc_line
{   int src, dest;
    int64_t weight;};

static struct adj_node *insert_a_node_in_adj_list(struct adj_node *adj, struct adj_node *new_node)
{
    struct adj_node *cur;
    *cur = (struct adj_node){0};
    for(cur->next = adj; cur->next != NULL && cur->next->weight < new_node->weight; cur = cur->next);
    ;
    new_node->next = cur->next;
    cur->next = new_node;
    if (new_node->next == adj) adj = new_node;
    return adj;
}

static void delete_all_lines_in_DGraph(struct DGraph_info *DGraph)
{
    struct adj_node *cur;
    for (size_t v = 0; v < NODE_NUM; v++)
    {
        cur = DGraph->inadj[v];
        while (cur != NULL)
        {
            struct adj_node *tmp = cur;
            cur = cur->next;
            free(tmp);
        }
        cur = DGraph->outadj[v];
        while (cur != NULL)
        {
            struct adj_node *tmp = cur;
            cur = cur->next;
            free(tmp);
        }
    }
    memset(DGraph->inadj, 0, NODE_NUM * 8UL);
    memset(DGraph->outadj, 0, NODE_NUM * 8UL);
    DGraph->line_num = 0;
    return;
}

int init_DGraph(struct DGraph_info *DGraph, struct dirc_line lines[], size_t line_num)
{
    for (size_t v = 0; v < NODE_NUM; v++)
    {
        DGraph->outadj[v] = NULL;
        DGraph->inadj[v] = NULL;
    }
    for (size_t e = 0; e < line_num; e++)
    {
        if (lines[e].src >= NODE_NUM || lines[e].dest >= NODE_NUM)
        {
            fputs("line node_id error. Fail to initialize directed graph!\n", stderr);
            delete_all_lines_in_DGraph(DGraph);
            return -1;
        }
        /* use weight-ascending order to creat an adjacency list */
        struct adj_node *new_src_node = (struct adj_node *)malloc(sizeof(struct adj_node));
        new_src_node->node_id = lines[e].dest;
        new_src_node->weight = lines[e].weight;
        new_src_node->next = NULL;
        if (DGraph->outadj[lines[e].src] == NULL)
            DGraph->outadj[lines[e].src] = new_src_node;
        else DGraph->outadj[lines[e].src] = insert_a_node_in_adj_list(DGraph->outadj[lines[e].src], new_src_node);
        /* use use weight-ascending order to creat a reverse adjacency list */
        struct adj_node *new_dest_node = (struct adj_node *)malloc(sizeof(struct adj_node));
        new_dest_node->node_id = lines[e].src;
        new_dest_node->weight = lines[e].weight;
        new_dest_node->next = NULL;
        if (DGraph->inadj[lines[e].dest] == NULL)
            DGraph->inadj[lines[e].dest] = new_dest_node;
        else DGraph->inadj[lines[e].dest] = insert_a_node_in_adj_list(DGraph->inadj[lines[e].dest], new_dest_node);
    }
    DGraph->line_num = line_num;
    return 0;
}

int delete_a_dirc_line_in_DGraph(struct DGraph_info *DGraph, int src, int dest)
{
    struct adj_node *cur, *last;
    last = NULL;
    for (cur = DGraph->outadj[src]; cur != NULL; cur = cur->next)
    {
        if (cur->node_id == dest)
        {
            if (last != NULL)
                last->next = cur->next;
            else DGraph->outadj[src] = cur->next;
            break;
        }
        last = cur;
    }
    last = NULL;
    for (cur = DGraph->inadj[dest]; cur != NULL; cur = cur->next)
    {
        if (cur->node_id == src)
        {
            if (last != NULL)
                last->next = cur->next;
            else DGraph->inadj[dest] = cur->next;
            break;
        }
        last = cur;
    }
    if (cur == NULL)
    {
        fprintf(stderr, "Fail to delete! Error: No correponding line from node %d to node %d.\n", src, dest);
        return -1;
    }
    else
    {
        DGraph->line_num--;
        free(cur);
        return 0;
    }
}

static int find_disjt_root(int *disjt_set, int node_id)
{
    if (disjt_set[node_id] == node_id)
        return node_id;
    else
    {
        disjt_set[node_id] = find_disjt_root(disjt_set, node_id);
        return disjt_set[node_id];
    }
}

static int Tarjan_algorithm_from_a_node_in_DGraph(const struct DGraph_info *DGraph, int node_id, int init_time, int *timestamp)
{
    timestamp[node_id] = init_time;
    struct adj_node *next_adj;
    for(next_adj = DGraph->outadj[node_id]; next_adj != NULL; next_adj = next_adj->next)
    {
        if (timestamp[next_adj->node_id] == -1)
        {
            Tarjan_algorithm_from_a_node_in_DGraph(DGraph, next_adj->node_id, init_time + 1);
            timestamp[node_id] = timestamp[next_adj->node_id] < timestamp[node_id] ? timestamp[next_adj->node_id] : timestamp[node_id];
        }
        else if (timestamp[next_adj->node_id] < timestamp[node_id])
            timestamp[node_id] = timestamp[next_adj->node_id];
    }
    return timestamp[node_id];
}

/* The stack of strongly connected component */
static int volatile SCC_stack[NODE_NUM];
/* The top of strongly connected stack */
static _Atomic(int) top_in_SCC_stack = -1;
/* find strongly connected components from a node */
static size_t get_SCC_from_a_node_in_DGraph(const struct DGraph_info *DGraph, int node_id, int init_time, int *timestamp, int *disjt_set)
{
    /* the number of strongly connected components */
    size_t SCC_num = 0;
    timestamp[node_id] = init_time;
    SCC_stack[++top_in_SCC_stack] = node_id;
    int cur_id;
    for(struct adj_node *next_adj = DGraph->outadj[node_id]; next_adj != NULL; next_adj = next_adj->next)
    {
        cur_id = next_adj->node_id;
        if (timestamp[cur_id] == -1)
        {
            SCC_num += get_SCC_from_a_node_in_DGraph(DGraph, cur_id, init_time + 1, disjt_set);
            timestamp[node_id] = timestamp[cur_id] < timestamp[node_id] ? timestamp[cur_id] : timestamp[node_id];
            disjt_set[node_id] = timestamp[cur_id] < timestamp[node_id] ? disjt_set[cur_id] : disjt_set[node_id];
        }
        /* if cur_id is still in stack */
        else if (timestamp[cur_id] <= top_in_SCC_stack)
            timestamp[node_id] = timestamp[cur_id] < timestamp[node_id] ? timestamp[cur_id] : timestamp[node_id];
            disjt_set[node_id] = timestamp[cur_id] < timestamp[node_id] ? disjt_set[cur_id] : disjt_set[node_id];
    }
    if (timestamp[node_id] == init_time)
    {
        fputs("Here is strongly connected component: ", stdout);
        while(SCC_stack[top_in_SCC_stack] != node_id)
            printf("%d ", SCC_stack[top_in_SCC_stack--]);
        printf("%d\n", SCC_stack[top_in_SCC_stack--]);
        return 1;
    }
    return SCC_num;
}

/* find all strongly connected components and output them,
and count the total number */
size_t find_all_SCC_in_DGraph(const struct DGraph_info *DGraph)
{
    /* timestamp in the traversal to the whole directed graph */
    int timestamp[NODE_NUM] = {-1};
    int disjt_set[NODE_NUM];
    for (int v = 0; v < NODE_NUM; v++)
        disjt_set[v] = v;
    /* the number of strongly connected components */
    size_t SCC_num = 0;
    for (int v = 0; v < NODE_NUM; v++)
        if (timestamp[v] == -1)
            SCC_num += get_SCC_from_a_node_in_DGraph(DGraph, v, 0, timestamp, disjt_set);
    return SCC_num;
}

/* a node in directed tree */
struct tree_node
{   int node_id;
    int64_t dist;
    struct tree_node **next;
    int parent_id;
    struct tree_node *parent;
    size_t child_num;};

static size_t insert_leaf_in_tree_node(struct tree_node *node, struct tree_node *new_leaf)
{
    if ((node->next = (struct tree_node **)realloc(node->next, (node->child_num + 1) * 8UL)) == NULL)
    {
        perror("fail to allocate array");
        exit(EXIT_FAILURE);
    }
    size_t pos = 0;
    if (node->child_num != 0)
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
        pos = left > middle ? left : middle;
        for (size_t i = pos; i < node->child_num; i++)
            node->next[i + 1] = node->next[i];
    }
    node->next[pos] = new_leaf;
    new_leaf->parent = node;
    node->child_num++;
    return pos;
}

static void delete_all_nodes_in_dirc_tree(struct tree_node *node)
{
    if (node->child_num == 0)
    {
        free(node); return;
    }
    for (size_t i = 0; i < node->child_num; i++)
        delete_all_nodes_in_dirc_tree(node->next[i]);
    free(node);
    return;
}

static struct tree_node *copy_to_dirc_shortest_list(struct tree_node *node)
{
    struct tree_node *list_node = (struct tree_node *)malloc(sizeof(struct tree_node));
    memset(list_node, 0, sizeof(struct tree_node));
    list_node->next = (struct tree_node **)malloc(8UL);
    list_node->child_num = 1;
    list_node->dist = node->dist;
    list_node->node_id = node->node_id;
    list_node->parent_id = node->parent_id;
    return list_node;
}