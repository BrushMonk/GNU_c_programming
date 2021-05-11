#pragma once
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "UDGraph.c"

static _Bool color_node_from_a_node_in_UDGraph(const struct UDGraph_info *UDGraph, int node_id, int init_color, int *color_set)
{
    color_set[node_id] = init_color >> 2;
    struct adj_multiline *adj_line = UDGraph->adj[node_id];
    while (adj_line != NULL)
    {
        int adj_id = (adj_line->i_node != node_id) ? adj_line->j_node : adj_line->i_node;
        if (color_set[adj_id] == -1)
            return color_node_from_a_node_in_UDGraph(UDGraph, adj_id, init_color + 1, color_set);
        else if (color_set[adj_id] != init_color >> 2)
            return 0;
        adj_line = (adj_line->i_node == node_id) ? adj_line->i_next : adj_line->j_next;
    }
    return 1;
}

_Bool is_a_bipartite(const struct UDGraph_info *UDGraph)
{
    int color_set[NODE_NUM];
    memset(color_set, -1, NODE_NUM * sizeof(int));
    for (int v = 0; v < NODE_NUM; v++)
        if (color_set[v] = -1 && color_node_from_a_node_in_UDGraph(UDGraph, v, 0, color_set) == 0)
            return 0;
    return 1;
}

struct undirc_line* Hungarian_algorithm_in_UDGraph(const struct UDGraph_info *UDGraph)
{
    if (is_a_bipartite(UDGraph) == 0)
    {
        fputs("The undirected graph is not a bipartite graph.\n", stderr);
        return NULL;
    }
}

struct undirc_line* Kuhn_Munkres_algorithm_in_UDGraph(const struct UDGraph_info *UDGraph)
{

}