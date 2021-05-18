#pragma once
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "UDGraph.c"

static int *nodex;
static size_t x_num = 0;
static int *nodey;
static size_t y_num = 0;

static _Bool color_nodes_from_a_node_in_UDGraph(const struct UDGraph_info *UDGraph, int node_id, _Bool init_color, int *color_set)
{
    color_set[node_id] = init_color;
    if (color_set[node_id] == 0)
    {
        nodex = (int *)realloc(nodex, ++x_num * sizeof(int));
        nodex[x_num - 1] = node_id;
    }
    else
    {
        nodey = (int *)realloc(nodey, ++y_num * sizeof(int));
        nodey[y_num - 1] = node_id;
    }
    struct adj_multiline *adj_line = UDGraph->adj[node_id];
    while (adj_line != NULL)
    {
        int adj_id = (adj_line->i_node != node_id) ? adj_line->j_node : adj_line->i_node;
        if (color_set[adj_id] == -1)
            return color_nodes_from_a_node_in_UDGraph(UDGraph, adj_id, !init_color, color_set);
        else if (color_set[adj_id] != init_color)
            return 0;
        adj_line = (adj_line->i_node == node_id) ? adj_line->i_next : adj_line->j_next;
    }
    return 1;
}

static _Bool is_bipartite(const struct UDGraph_info *UDGraph)
{
    int color_set[NODE_NUM] = {-1};
    for (int v = 0; v < NODE_NUM; v++)
        if (color_set[v] == -1 && color_nodes_from_a_node_in_UDGraph(UDGraph, v, 0, color_set) == 0)
        {
            free(nodex); free(nodey);
            x_num = 0; y_num = 0;
            return 0;
        }
    return 1;
}

/* a matching in undircted graph */
struct matching
{   struct adj_multiline **matched_line;
    size_t line_num;};

struct adj_multiline *get_match_line(const struct UDGraph_info *UDGraph, int node_id)
{
    struct adj_multiline *adj_line = UDGraph->adj[node_id];
    while (adj_line != NULL && adj_line->ismarked == 0)
        adj_line = (adj_line->i_node == node_id) ? adj_line->i_next : adj_line->j_next;
    return adj_line;
}

size_t find_augmenting_path(const struct UDGraph_info *UDGraph, int node_id, _Bool *isvisited)
{
    struct adj_multiline *adj_line = UDGraph->adj[node_id];
    while (adj_line != NULL)
    {
        int adj_id = (adj_line->i_node != node_id) ? adj_line->i_node : adj_line->j_node;
        if (!isvisited[adj_id])
        {
            isvisited[adj_id] = 1;
            struct adj_multiline *adj_match_line = get_match_line(UDGraph, adj_id);
            int adj_match = -1;
            if (adj_match_line != NULL)
                adj_match = (adj_match_line->i_node != adj_id) ? adj_match_line->i_node : adj_match_line->j_node;
            if (adj_match_line == NULL || find_augmenting_path(UDGraph, adj_match, isvisited))
            {
                if (adj_match_line != NULL)
                    adj_match_line->ismarked = 0;
                adj_line->ismarked = 1;
                return 1;
            }
        }
        adj_line = (adj_line->i_node == node_id) ? adj_line->i_next : adj_line->j_next;
    }
    return 0;
}

struct matching *Hungarian_algorithm_in_UDGraph(const struct UDGraph_info *UDGraph)
{
    if (is_bipartite(UDGraph) == 0)
    {
        fputs("The undirected graph is not bipartite.\n", stderr);
        return NULL;
    }
    struct matching *max_matching;
    *max_matching = (struct matching){0, 0};
    _Bool isvisited[NODE_NUM] = {0};
    for (size_t i = 0; i < x_num; i++)
    {
        memset(isvisited, 0, sizeof(isvisited));
        if (get_match_line(UDGraph, nodex[i]) == NULL)
            max_matching->line_num += find_augmenting_path(UDGraph, nodex[i], isvisited);
    }
    max_matching->matched_line = (struct adj_multiline **)malloc(max_matching->line_num * 8UL);
    memset(isvisited, 0, sizeof(isvisited));
    for (size_t v = 0, e = 0; v < NODE_NUM; v++)
    {
        if (!isvisited[v])
        {
            struct adj_multiline *cur = UDGraph->adj[v];
            while (cur != NULL && cur->ismarked == 0)
                cur = cur->i_node == v ? cur->i_next : cur->j_next;
            if (cur != NULL)
            {
                max_matching->matched_line[e++] = cur;
                isvisited[cur->i_node] = isvisited[cur->j_node] = 1;
            }
        }
    }
    return max_matching;
}

struct matching* Kuhn_Munkres_algorithm_in_UDGraph(const struct UDGraph_info *UDGraph)
{
    if (is_bipartite(UDGraph) == 0)
    {
        fputs("The undirected graph is not bipartite.\n", stderr);
        return NULL;
    }
    struct matching *opti_matching;
    *opti_matching = (struct matching){0, 0};
    return opti_matching;
}