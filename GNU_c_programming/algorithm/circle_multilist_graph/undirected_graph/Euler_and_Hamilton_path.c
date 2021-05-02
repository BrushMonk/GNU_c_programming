#pragma once
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "shortest_path_in_UDGraph.c"

struct tree_node *Chinese_postman_problem(const struct UDGraph_info *UDGraph)
{
    size_t odd_degree_num = 0;
    int *odd_degree_node;
    for (size_t v = 0; v < NODE_NUM; v++)
        if (UDGraph->degree[v] >> 1 == 1)
        {
            odd_degree_num++;
            odd_degree_node = (int *)realloc(odd_degree_node, odd_degree_num * sizeof(int));
            odd_degree_num[odd_degree_num - 1] = v;
        }
    struct tree_node *dist_path[odd_degree_num * (odd_degree_num - 1) / 2];
    for (size_t v = 0, i = 0; v < odd_degree_num; v++)
        for (size_t n = v + 1; n < odd_degree_num; n++)
            dist_path[i++] = Dijkstra_algorithm_in_UDGraph(UDGraph, v, n);

}