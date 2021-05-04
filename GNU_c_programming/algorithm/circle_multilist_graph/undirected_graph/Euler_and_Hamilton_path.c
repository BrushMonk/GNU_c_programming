#pragma once
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "shortest_path_in_UDGraph.c"

static int find_next_nodeid_in_UDGraph_Euler_path(const struct UDGraph_info *UDGraph, int node_id, int64_t *dist, size_t *used_degree)
{
    if (used_degree[node_id] == UDGraph->degree[node_id])
        return -1;
    struct adj_multiedge *chosen;
    struct adj_multiedge *cur = UDGraph->closest_adj[node_id];
    while (cur != NULL)
    {
        /* next candidate node id in all adjacenct nodes */
        int cand_id = cur->i_node == node_id ? cur->j_node : cur->i_node;
        if (cur->ismarked == 0 && UDGraph->degree[cand_id] - used_degree[cand_id] > 1 && )
        {
            chosen = cur;
            break;
        }
        else
        {
            if (cur->ismarked == 0)
                chosen = cur;
            cur = cur->i_node == node_id ? cur->i_next : cur->j_next;
            continue;
        }
    }
    chosen->ismarked = 1;
    *dist = chosen->weight;
    int next_id = chosen->i_node == node_id ? chosen->j_node : chosen->i_node;
    used_degree[next_id]++;
    used_degree[node_id]++;
    return next_id;
}

struct tree_node *Fleury_algorithm_in_UDGraph(const struct UDGraph_info *UDGraph, int src)
{
    size_t used_degree[NODE_NUM] = {0};
    struct tree_node *start_node = (struct tree_node){0};
    start_node->parent_id = -1;
    start_node->node_id = src;
    struct tree_node *last = NULL, path_node;
    int cur_id = src;
    int64_t dist = 0;
    while(cur_id == -1)
    {
        if (last != NULL)
        {
            int64_t dist = 0;
            *path_node = (struct tree_node){cur_id, dist, 0};
            insert_leaf_in_tree_node(last, path_node);
        }
        else path_node = start_node;
        last = path_node;
        cur_id = find_next_nodeid_in_UDGraph_Euler_path(UDGraph, cur_id, &dist, used_degree);
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
    if ( odd_deg_num == 0 || (odd_deg_num == 2 && UDGraph->degree[src] >> 1 == 1) )
        return Fleury_algorithm_in_UDGraph(UDGraph, src);
    struct tree_node *dist_path[odd_deg_num * (odd_deg_num - 1) / 2];
    for (size_t v = 0, i = 0; v < odd_deg_num; v++)
        for (size_t n = v + 1; n < odd_deg_num; n++)
            dist_path[i++] = Dijkstra_algorithm_in_UDGraph(UDGraph, odd_deg_node[v], odd_deg_node[n]);

}