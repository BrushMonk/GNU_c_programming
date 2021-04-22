#pragma once
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
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
    for (size_t count = 0; count < NODE_NUM; count++)
    {
        cur = DGraph->closest_inadj[count];
        while (cur != NULL)
        {
            struct adj_node *tmp = cur;
            cur = cur->next;
            free(tmp);
        }
        cur = DGraph->closest_outadj[count];
        while (cur != NULL)
        {
            struct adj_node *tmp = cur;
            cur = cur->next;
            free(tmp);
        }
    }
    free(DGraph->closest_inadj);
    free(DGraph->closest_outadj);
    DGraph->side_num = 0;
    return;
}

int init_DGraph(struct DGraph_info *DGraph, struct dirc_side_info side_list[], size_t side_num)
{
    DGraph->closest_outadj = (struct adj_node **)malloc(NODE_NUM * 8UL);
    DGraph->closest_inadj = (struct adj_node **)malloc(NODE_NUM * 8UL);
    for (size_t count = 0; count < NODE_NUM; count++)
    {
        DGraph->closest_outadj[count] = NULL;
        DGraph->closest_inadj[count] = NULL;
    }
    for (size_t count = 0; count < side_num; count++)
    {
        if (side_list[count].src >= NODE_NUM || side_list[count].dest >= NODE_NUM)
        {
            fputs("side node_id error. Fail to initialize directed graph!\n", stderr);
            delete_all_sides_in_DGraph(DGraph);
            return -1;
        }
        /* use weight-ascending order to creat an adjacency list */
        struct adj_node *new_src_node = (struct adj_node *)malloc(sizeof(struct adj_node));
        new_src_node->node_id = side_list[count].dest;
        new_src_node->weight = side_list[count].weight;
        new_src_node->next = NULL;
        if (DGraph->closest_outadj[side_list[count].src] == NULL)
            DGraph->closest_outadj[side_list[count].src] = new_src_node;
        else DGraph->closest_outadj[side_list[count].src] = insert_a_node_in_adj_list(DGraph->closest_outadj[side_list[count].src], new_src_node);
        /* use use weight-ascending order to creat a reverse adjacency list */
        struct adj_node *new_dest_node = (struct adj_node *)malloc(sizeof(struct adj_node));
        new_dest_node->node_id = side_list[count].src;
        new_dest_node->weight = side_list[count].weight;
        new_dest_node->next = NULL;
        if (DGraph->closest_inadj[side_list[count].dest] == NULL)
            DGraph->closest_inadj[side_list[count].dest] = new_dest_node;
        else DGraph->closest_inadj[side_list[count].dest] = insert_a_node_in_adj_list(DGraph->closest_inadj[side_list[count].dest], new_dest_node);
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

struct DGraph_info *Tarjan_algorithm_in_DGraph(struct DGraph_info *DGraph)
{
    
}