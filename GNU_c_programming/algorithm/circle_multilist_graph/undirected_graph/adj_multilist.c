#pragma once
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#define NODE_NUM 1024
/* adjacency multilist node */
struct adj_multinode
{   int i_node, j_node;
    struct adj_multinode *i_next, *j_next;
    int64_t weight;
    _Bool ismarked;};

/* weighted undirected graph infomation */
struct UDGraph_info
{
    /* the closest adjacency node */
    struct adj_multinode **closest_adj;
    size_t side_num;
};

struct undirc_side_info
{   int i_node, j_node;
    int64_t weight;};

static struct adj_multinode *insert_a_node_in_adj_multilist(struct adj_multinode *closest_adj, struct adj_multinode *new_node)
{
    struct adj_multinode *cur;
    *cur =  (struct adj_multinode){0};
    if (closest_adj->i_node == new_node->i_node)
    {
        for(cur->i_next = closest_adj; cur->i_next != NULL && cur->i_next->weight < new_node->weight; cur = cur->i_next);
        ;
        new_node->i_next = cur->i_next;
        cur->i_next = new_node;
        if (new_node->i_next == closest_adj) closest_adj = new_node;
        return closest_adj;
    }
    if (closest_adj->j_node == new_node->j_node)
    {
        for(cur->j_next = closest_adj; cur->j_next != NULL && cur->j_next->weight < new_node->weight; cur = cur->j_next);
        ;
        new_node->j_next = cur->j_next;
        cur->j_next = new_node;
        if (new_node->j_next == closest_adj) closest_adj = new_node;
        return closest_adj;
    }
    else
    {
        fputs("error: Non-existent node in undirected graph!\n", stderr);
        return NULL;
    }
}

static void delete_all_sides_in_UDGraph(struct UDGraph_info *UDGraph)
{
    for (size_t count = 0; count < NODE_NUM; count++)
    {
        struct adj_multinode *cur = UDGraph->closest_adj[count];
        while (cur != NULL)
        {
            struct adj_multinode *tmp = cur;
            if (cur->i_node == count)
            {
                if (UDGraph->closest_adj[cur->j_node] == cur)
                    UDGraph->closest_adj[cur->j_node] = UDGraph->closest_adj[cur->j_node]->j_next;
                cur = cur->i_next;
            }
            if (cur->j_node == count)
            {
                if (UDGraph->closest_adj[cur->i_node] == cur)
                    UDGraph->closest_adj[cur->i_node] = UDGraph->closest_adj[cur->i_node]->i_next;
                cur = cur->j_next;
            }
            free(tmp);
        }
    }
    memset(UDGraph->closest_adj, 0, NODE_NUM * 8UL);
    UDGraph->side_num = 0;
    return;
}

int init_UDGraph(struct UDGraph_info *UDGraph, struct undirc_side_info side_list[], size_t side_num)
{
    UDGraph->closest_adj = (struct adj_multinode **)malloc(NODE_NUM * 8UL);
    for (size_t count = 0; count < NODE_NUM; count++)
        UDGraph->closest_adj[count] = NULL;
    for (size_t count = 0; count < side_num; count++)
    {
        if (side_list[count].i_node >= NODE_NUM || side_list[count].j_node >= NODE_NUM)
        {
            fputs("side node_id error. Fail to initialize undirected graph!\n", stderr);
            delete_all_sides_in_UDGraph(UDGraph);
            UDGraph = NULL;
            return -1;
        }
        /* use weight-ascending order to creat an adjacency multilist */
        struct adj_multinode *new_node = (struct adj_multinode *)malloc(sizeof(struct adj_multinode));
        memset(new_node, 0, sizeof(struct adj_multinode));
        new_node->i_node = side_list[count].i_node;
        new_node->j_node = side_list[count].j_node;
        new_node->weight = side_list[count].weight;
        if (UDGraph->closest_adj[side_list[count].i_node] == NULL)
            UDGraph->closest_adj[side_list[count].i_node] = new_node;
        else UDGraph->closest_adj[side_list[count].i_node] = insert_a_node_in_adj_multilist(UDGraph->closest_adj[side_list[count].i_node], new_node);
        if (UDGraph->closest_adj[side_list[count].j_node] == NULL)
            UDGraph->closest_adj[side_list[count].j_node] = new_node;
        else UDGraph->closest_adj[side_list[count].j_node] = insert_a_node_in_adj_multilist(UDGraph->closest_adj[side_list[count].j_node], new_node);
    }
    UDGraph->side_num = side_num;
    return 0;
}

int delete_a_undirc_side_in_UDGraph(struct UDGraph_info *UDGraph, int node1, int node2)
{
    struct adj_multinode *cur, *last;
    cur = UDGraph->closest_adj[node1]; last = NULL;
    while (cur != NULL)
    {
        if (cur->j_node == node2)
        {
            if (last != NULL && last->i_next == cur)
                last->i_next = cur->i_next;
            else if (last != NULL &&  last->j_next == cur)
                last->j_next = cur->i_next;
            else UDGraph->closest_adj[node1] = cur->i_next;
            break;
        }
        if (cur->i_node == node2)
        {
            if (last != NULL && last->i_next == cur)
                last->i_next = cur->j_next;
            else if (last != NULL &&  last->j_next == cur)
                last->j_next = cur->j_next;
            else UDGraph->closest_adj[node1] = cur->j_next;
            break;
        }
        last = cur;
        cur = (cur->i_node == node1) ? cur->i_next : cur->j_next;
    }
    cur = UDGraph->closest_adj[node2]; last = NULL;
    while (cur != NULL)
    {
        if (cur->j_node == node1)
        {
            if (last != NULL && last->i_next == cur)
                last->i_next = cur->i_next;
            else if (last != NULL &&  last->j_next == cur)
                last->j_next = cur->i_next;
            else UDGraph->closest_adj[node1] = cur->i_next;
            break;
        }
        if (cur->i_node == node1)
        {
            if (last != NULL && last->i_next == cur)
                last->i_next = cur->j_next;
            else if (last != NULL &&  last->j_next == cur)
                last->j_next = cur->j_next;
            else UDGraph->closest_adj[node1] = cur->j_next;
            break;
        }
        last = cur;
        cur = (cur->i_node == node2) ? cur->i_next : cur->j_next;
    }
    if (cur != NULL)
    {
        UDGraph->side_num--;
        free(cur); return 0;
    }
    else
    {
        fprintf(stderr, "Fail to delete! Error: No undirected side linking with node %d and %d.\n", node1, node2);
        return -1;
    }
}