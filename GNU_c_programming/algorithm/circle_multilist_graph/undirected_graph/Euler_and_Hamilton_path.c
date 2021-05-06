#pragma once
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "shortest_path_in_UDGraph.c"

static int find_next_nodeid_in_undirc_Euler_path(struct UDGraph_info *UDGraph, int node_id, int64_t *dist)
{
    struct adj_multiline *adj_line = UDGraph->closest_adj[node_id];
    int next_id = -1;
    while (adj_line != NULL)
    {
        /* next node id in all adjacenct nodes */
        int adj_id = adj_line->i_node == node_id ? adj_line->j_node : adj_line->i_node;
        if (is_a_bridge_in_UDGraph(UDGraph, adj_line->i_node, adj_line->j_node))
            next_id = adj_id;
        /* if the adjacenct line is not a bridge, jump out of this loop. */
        else
        {
            next_id = adj_id;
            break;
        }
        adj_line = (adj_line->i_node == node->node_id) ? adj_line->i_next : adj_line->j_next;
    }
    *dist = adj_line->weight;
    if (adj_line != NULL)
        delete_a_undirc_line_in_UDGraph(UDGraph, (struct undirc_line){adj_line->i_node, adj_line->j_node, adj_line->weight});
    return next_id;
}

struct tree_node *Fleury_algorithm_in_UDGraph(const struct UDGraph_info *UDGraph, int src)
{
    struct adj_multiline **line_set = get_lines_set_in_ascd_order(UDGraph);
    struct undirc_line lines[UDGraph->line_num];
    for (size_t e = 0; e < UDGraph->line_num; e++)
    {
        lines[e].i_node = line_set[e]->i_node;
        lines[e].j_node = line_set[e]->j_node;
        lines[e].weight = line_set[e]->weight;
    }
    struct UDGraph_info *unvis_UDGraph = (struct UDGraph_info *)malloc(sizeof(struct UDGraph_info));
    init_UDGraph(unvis_UDGraph, lines, UDGraph->line_num);
    struct tree_node *start_node = (struct tree_node){src, 0, 0, -1, 0};
    struct tree_node *last = NULL, *path_node;
    int cur_id = src;
    int64_t dist = 0;
    while (cur_id == -1)
    {
        if (last != NULL)
        {
            *path_node = (struct tree_node){cur_id, dist, 0, 0, 0};
            insert_leaf_in_tree_node(last, path_node);
        }
        else path_node = start_node;
        last = path_node;
        cur_id = find_next_nodeid_in_undirc_Euler_path(unvis_UDGraph, cur_id, &dist);
    }
    free(unvis_UDGraph);
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
    else
    {
        struct tree_node *dist_path[odd_deg_num * (odd_deg_num - 1) / 2];
        int64_t dist[odd_deg_num * (odd_deg_num - 1) / 2];
        for (size_t v = 0, i = 0; v < odd_deg_num; v++)
            for (size_t n = v + 1; n < odd_deg_num; n++)
                dist_path[i++] = Dijkstra_algorithm_in_UDGraph(UDGraph, odd_deg_node[v], odd_deg_node[n], &dist[i]);
    }
}