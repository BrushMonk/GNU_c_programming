#pragma once
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "shortest_path_in_UDGraph.c"

struct tree_node *Fleury_algorithm_in_UDGraph(const struct UDGraph_info *UDGraph, int src)
{
    struct tree_node *start_node = (struct tree_node){0};
    start_node->parent_id = -1;
    start_node->node_id = src;
    struct adj_multinode *cur;
    struct tree_node *last = NULL, path_node;
    for (int v = src, e = 0; e < UDGraph->side_num;)
    {
        if (last != NULL)
        {
            *path_node = (struct tree_node){(v, cur->weight, 0});
            insert_leaf_in_tree_node(last, path_node);
        }
        else path_node = start_node;
        last = path_node;
        cur = UDGraph->closest_adj[v];
        while (cur != NULL)
        {
            if (cur->ismarked == 0)
            {
                cur->ismarked = 1;
                e++;
                v = cur->i_node == v ? cur->j_node : cur->i_node;
                break;
            }
            else cur = cur->i_node == v ? cur->i_next : cur->j_next;
        }
    }
    return start_node;
}

struct tree_node *Chinese_postman_problem(const struct UDGraph_info *UDGraph, int src)
{
    size_t odd_deg_num = 0;
    int *odd_deg_node;
    for (size_t v = 0; v < NODE_NUM; v++)
        if (UDGraph->degree[v] >> 1 == 1)
        {
            odd_deg_num++;
            odd_deg_node = (int *)realloc(odd_deg_node, odd_deg_num * sizeof(int));
            odd_deg_num[odd_deg_num - 1] = v;
        }
    if (odd_deg_num == 0)
        return Fleury_algorithm_in_UDGraph(UDGraph, src);
    if (odd_deg_num == 2 && UDGraph->degree[src] >> 1 == 1)
    {
    }
    struct tree_node *dist_path[odd_deg_num * (odd_deg_num - 1) / 2];
    for (size_t v = 0, i = 0; v < odd_deg_num; v++)
        for (size_t n = v + 1; n < odd_deg_num; n++)
            dist_path[i++] = Dijkstra_algorithm_in_UDGraph(UDGraph, odd_deg_node[v], odd_deg_node[n]);

}