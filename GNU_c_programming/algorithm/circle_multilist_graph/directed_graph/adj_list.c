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
    struct adj_node **closest_outadj;
    /* the closest indegree adjacency node */
    struct adj_node **closest_inadj;
    size_t side_num;
};

struct dirc_side_info
{   int src, dest;
    int64_t weight;};

static struct adj_node *insert_a_node_in_adj_list(struct adj_node *closest_adj, struct adj_node *new_node)
{
    struct adj_node *cur;
    *cur = (struct adj_node){0};
    for(cur->next = closest_adj; cur->next != NULL && cur->next->weight < new_node->weight; cur = cur->next);
    ;
    new_node->next = cur->next;
    cur->next = new_node;
    if (new_node->next == closest_adj) closest_adj = new_node;
    return closest_adj;
}

static void delete_all_sides_in_DGraph(struct DGraph_info *DGraph)
{
    struct adj_node *cur;
    for (size_t v = 0; v < NODE_NUM; v++)
    {
        cur = DGraph->closest_inadj[v];
        while (cur != NULL)
        {
            struct adj_node *tmp = cur;
            cur = cur->next;
            free(tmp);
        }
        cur = DGraph->closest_outadj[v];
        while (cur != NULL)
        {
            struct adj_node *tmp = cur;
            cur = cur->next;
            free(tmp);
        }
    }
    memset(DGraph->closest_inadj, 0, NODE_NUM * 8UL);
    memset(DGraph->closest_outadj, 0, NODE_NUM * 8UL);
    DGraph->side_num = 0;
    return;
}

int init_DGraph(struct DGraph_info *DGraph, struct dirc_side_info side_list[], size_t side_num)
{
    DGraph->closest_outadj = (struct adj_node **)malloc(NODE_NUM * 8UL);
    DGraph->closest_inadj = (struct adj_node **)malloc(NODE_NUM * 8UL);
    for (size_t v = 0; v < NODE_NUM; v++)
    {
        DGraph->closest_outadj[v] = NULL;
        DGraph->closest_inadj[v] = NULL;
    }
    for (size_t v = 0; v < side_num; v++)
    {
        if (side_list[v].src >= NODE_NUM || side_list[v].dest >= NODE_NUM)
        {
            fputs("side node_id error. Fail to initialize directed graph!\n", stderr);
            delete_all_sides_in_DGraph(DGraph);
            return -1;
        }
        /* use weight-ascending order to creat an adjacency list */
        struct adj_node *new_src_node = (struct adj_node *)malloc(sizeof(struct adj_node));
        new_src_node->node_id = side_list[v].dest;
        new_src_node->weight = side_list[v].weight;
        new_src_node->next = NULL;
        if (DGraph->closest_outadj[side_list[v].src] == NULL)
            DGraph->closest_outadj[side_list[v].src] = new_src_node;
        else DGraph->closest_outadj[side_list[v].src] = insert_a_node_in_adj_list(DGraph->closest_outadj[side_list[v].src], new_src_node);
        /* use use weight-ascending order to creat a reverse adjacency list */
        struct adj_node *new_dest_node = (struct adj_node *)malloc(sizeof(struct adj_node));
        new_dest_node->node_id = side_list[v].src;
        new_dest_node->weight = side_list[v].weight;
        new_dest_node->next = NULL;
        if (DGraph->closest_inadj[side_list[v].dest] == NULL)
            DGraph->closest_inadj[side_list[v].dest] = new_dest_node;
        else DGraph->closest_inadj[side_list[v].dest] = insert_a_node_in_adj_list(DGraph->closest_inadj[side_list[v].dest], new_dest_node);
    }
    DGraph->side_num = side_num;
    return 0;
}

int delete_a_dirc_side_in_DGraph(struct DGraph_info *DGraph, int src, int dest)
{
    struct adj_node *cur, *last;
    last = NULL;
    for (cur = DGraph->closest_outadj[src]; cur != NULL; cur = cur->next)
    {
        if (cur->node_id == dest)
        {
            if (last != NULL)
                last->next = cur->next;
            else DGraph->closest_outadj[src] = cur->next;
            break;
        }
        last = cur;
    }
    last = NULL;
    for (cur = DGraph->closest_inadj[dest]; cur != NULL; cur = cur->next)
    {
        if (cur->node_id == src)
        {
            if (last != NULL)
                last->next = cur->next;
            else DGraph->closest_inadj[dest] = cur->next;
            break;
        }
        last = cur;
    }
    if (cur == NULL)
    {
        fprintf(stderr, "Fail to delete! Error: No correponding side from node %d to node %d.\n", src, dest);
        return -1;
    }
    else
    {
        DGraph->side_num--;
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

/* timestamp in the traversal to the whole directed graph */
static int volatile timestamp[NODE_NUM] = {-1};
void DFS_from_a_node_in_in_DGraph(struct DGraph_info *DGraph, int node_id, int orin_time)
{
    timestamp[node_id] = orin_time;
    struct adj_node *next_adj;
    for(next_adj = DGraph->closest_outadj[node_id]; next_adj != NULL; next_adj = next_adj->next)
    {
        if (timestamp[next_adj->node_id] == -1)
            DFS_from_a_node_in_in_DGraph(DGraph, next_adj->node_id, orin_time + 1);
    }
    if (next_adj != NULL) printf("%d\040", node_id);
    return;
}

/* The stack of strongly connected component */
static int volatile SCC_stack[NODE_NUM];
/* The top of strongly connected stack */
static _Atomic(int) top_in_SCC_stack = -1;
int Tarjan_algorithm_from_a_node_in_DGraph(struct DGraph_info *DGraph, int node_id, int orin_time, int *disjt_set)
{
    SCC_stack[++top_in_SCC_stack] = node_id;
    timestamp[node_id] = orin_time;
    int cur_id;
    for(struct adj_node *next_adj = DGraph->closest_outadj[node_id]; next_adj != NULL; next_adj = next_adj->next)
    {
        cur_id = next_adj->node_id;
        if (timestamp[cur_id] == -1)
        {
            Tarjan_algorithm_from_a_node_in_DGraph(DGraph, cur_id, orin_time + 1, disjt_set);
            timestamp[node_id] = timestamp[cur_id] < timestamp[node_id] ? timestamp[cur_id] : timestamp[node_id];
            disjt_set[node_id] = timestamp[cur_id] < timestamp[node_id] ? disjt_set[cur_id] : disjt_set[node_id];
        }
        /* if cur_id is still in stack */
        else if (timestamp[cur_id] <= top_in_SCC_stack)
            timestamp[node_id] = timestamp[cur_id] < timestamp[node_id] ? timestamp[cur_id] : timestamp[node_id];
            disjt_set[node_id] = timestamp[cur_id] < timestamp[node_id] ? disjt_set[cur_id] : disjt_set[node_id];
    }
    if (timestamp[node_id] == orin_time)
    {
        
        fputs("Here is strongly connected component: ", stdout);
        while(SCC_stack[top_in_SCC_stack] != node_id)
            printf("%d ", SCC_stack[top_in_SCC_stack--]);
        printf("%d\n", SCC_stack[top_in_SCC_stack--]);
    }
    return timestamp[node_id];
}

/* find all strongly connected components and output them,
and count the total number */
size_t find_all_SCC_in_DGraph(struct DGraph_info *DGraph)
{
    int disjt_set[NODE_NUM];
    for (int v = 0; v < NODE_NUM; v++)
    {
        disjt_set[v] = v;
        timestamp[v] = -1;
    }
    size_t scc_num;
    for (int v = 0; v < NODE_NUM; v++)
        if (timestamp[v] == -1)
        {
            Tarjan_algorithm_from_a_node_in_DGraph(DGraph, v, 0, disjt_set);
            scc_num++;
        }
    return scc_num;
}