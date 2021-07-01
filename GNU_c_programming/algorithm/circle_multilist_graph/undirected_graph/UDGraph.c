#pragma once
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#define NODE_NUM INT16_MAX
/* adjacency multilist line */
struct adj_line
{   int i_node, j_node;
    struct adj_line *i_next, *j_next;
    int64_t weight;
    _Atomic(_Bool) ismarked;};

/* weighted undirected graph infomation */
struct UDGraph_info
{
    /* the closest adjacency node */
    struct adj_line *adj[NODE_NUM];
    size_t degree[NODE_NUM];
    size_t line_num;
};

struct undirc_line
{   int i_node, j_node;
    int64_t weight;};

static void delete_all_lines_in_UDGraph(struct UDGraph_info *UDGraph)
{
    for (size_t v = 0; v < NODE_NUM; v++)
    {
        struct adj_line *cur = UDGraph->adj[v];
        while (cur != NULL)
        {
            struct adj_line *tmp = cur;
            if (cur->i_node == v)
            {
                if (UDGraph->adj[cur->j_node] == cur)
                    UDGraph->adj[cur->j_node] = UDGraph->adj[cur->j_node]->j_next;
                cur = cur->i_next;
            }
            if (cur->j_node == v)
            {
                if (UDGraph->adj[cur->i_node] == cur)
                    UDGraph->adj[cur->i_node] = UDGraph->adj[cur->i_node]->i_next;
                cur = cur->j_next;
            }
            free(tmp);
        }
        UDGraph->adj[v] = NULL;
    }
    memset(UDGraph->adj, 0, NODE_NUM * 8UL);
    memset(UDGraph->degree, 0, NODE_NUM * sizeof(size_t));
    UDGraph->line_num = 0;
    return;
}

static int add_a_line_in_UDGraph(struct UDGraph_info *UDGraph, struct undirc_line line)
{
    if (line.i_node >= NODE_NUM || line.j_node >= NODE_NUM || line.i_node < 0 || line.j_node < 0)
    {
        fputs("line node_id error. Fail to initialize undirected graph!\n", stderr);
        return -1;
    }
    /* use weight-ascending order to creat an adjacency multilist */
    struct adj_line *new_line = (struct adj_line *)malloc(sizeof(struct adj_line));
    memset(new_line, 0, sizeof(struct adj_line));
    new_line->i_node = line.i_node;
    new_line->j_node = line.j_node;
    new_line->weight = line.weight;
    struct adj_line *cur, *last;

    cur = UDGraph->adj[line.i_node], last = NULL;
    while (cur != NULL && cur->weight < line.weight)
    {
        last = cur;
        cur = (cur->i_node == line.i_node) ? cur->i_next : cur->j_next;
    }
    new_line->i_next = cur;
    if (last != NULL && last->i_node == line.i_node)
        last->i_next = new_line;
    else if (last != NULL && last->j_node == line.i_node)
        last->j_next = new_line;
    else UDGraph->adj[line.i_node] = new_line;

    cur = UDGraph->adj[line.j_node]; last = NULL;
    while (cur != NULL && cur->weight < line.weight)
    {
        last = cur;
        cur = (cur->i_node == line.j_node) ? cur->i_next : cur->j_next;
    }
    new_line->j_next = cur;
    if (last != NULL && last->i_node == line.j_node)
        last->i_next = new_line;
    else if (last != NULL && last->j_node == line.j_node)
        last->j_next = new_line;
    else UDGraph->adj[line.j_node] = new_line;

    UDGraph->degree[line.i_node]++;
    UDGraph->degree[line.j_node]++;
    UDGraph->line_num++;
    return 0;
}

int init_UDGraph(struct UDGraph_info *UDGraph, struct undirc_line lines[], size_t line_num)
{
    for (size_t v = 0; v < NODE_NUM; v++)
        UDGraph->adj[v] = NULL;
    for (size_t e = 0; e < line_num; e++)
    {
        if (add_a_line_in_UDGraph(UDGraph, lines[e]) == -1)
        {
            delete_all_lines_in_UDGraph(UDGraph);
            UDGraph = NULL;
            exit(-1);
        }
    }
    return 0;
}

int delete_a_line_in_UDGraph(struct UDGraph_info *UDGraph, struct undirc_line line)
{
    struct adj_line *cur, *last;
    cur = UDGraph->adj[line.i_node]; last = NULL;
    while (cur != NULL)
    {
        if (cur->j_node == line.j_node && cur->weight == line.weight)
        {
            if (last != NULL && last->i_next == cur)
                last->i_next = cur->i_next;
            else if (last != NULL &&  last->j_next == cur)
                last->j_next = cur->i_next;
            else UDGraph->adj[line.i_node] = cur->i_next;
            break;
        }
        if (cur->i_node == line.j_node && cur->weight == line.weight)
        {
            if (last != NULL && last->i_next == cur)
                last->i_next = cur->j_next;
            else if (last != NULL &&  last->j_next == cur)
                last->j_next = cur->j_next;
            else UDGraph->adj[line.i_node] = cur->j_next;
            break;
        }
        last = cur;
        cur = (cur->i_node == line.i_node) ? cur->i_next : cur->j_next;
    }
    cur = UDGraph->adj[line.j_node]; last = NULL;
    while (cur != NULL)
    {
        if (cur->j_node == line.i_node && cur->weight == line.weight)
        {
            if (last != NULL && last->i_next == cur)
                last->i_next = cur->i_next;
            else if (last != NULL &&  last->j_next == cur)
                last->j_next = cur->i_next;
            else UDGraph->adj[line.j_node] = cur->i_next;
            break;
        }
        if (cur->i_node == line.i_node && cur->weight == line.weight)
        {
            if (last != NULL && last->i_next == cur)
                last->i_next = cur->j_next;
            else if (last != NULL &&  last->j_next == cur)
                last->j_next = cur->j_next;
            else UDGraph->adj[line.j_node] = cur->j_next;
            break;
        }
        last = cur;
        cur = (cur->i_node == line.j_node) ? cur->i_next : cur->j_next;
    }
    if (cur != NULL)
    {
        UDGraph->line_num--;
        UDGraph->degree[line.i_node]--;
        UDGraph->degree[line.j_node]--;
        free(cur); return 0;
    }
    else
    {
        fprintf(stderr, "Fail to delete! Error: No undirected line linking with node %d and %d.\n", line.i_node, line.j_node);
        return -1;
    }
}

static int Tarjan_algorithm_from_a_node_in_UDGraph(const struct UDGraph_info *UDGraph, int node_id, int init_time, int *timestamp)
{
    timestamp[node_id] = init_time;
    struct adj_line *adj_line = UDGraph->adj[node_id];
    while (adj_line != NULL)
    {
        int adj_id = (adj_line->i_node != node_id) ? adj_line->i_node : adj_line->j_node;
        if (timestamp[adj_id] == -1)
        {
            Tarjan_algorithm_from_a_node_in_UDGraph(UDGraph, adj_id, init_time + 1, timestamp);
            timestamp[node_id] = timestamp[adj_id] < timestamp[node_id] ? timestamp[adj_id] : timestamp[node_id];
        }
        else if (timestamp[adj_id] < timestamp[node_id])
            timestamp[node_id] = timestamp[adj_id];
        adj_line = (adj_line->i_node == node_id) ? adj_line->i_next : adj_line->j_next;
    }
    return timestamp[node_id];
}

static _Bool is_a_bridge_in_UDGraph(const struct UDGraph_info *UDGraph, int node_id1, int node_id2)
{
    /* timestamp in the traversal to the whole undirected graph */
    int timestamp[NODE_NUM] = {-1};
    Tarjan_algorithm_from_a_node_in_UDGraph(UDGraph, node_id1, 0, timestamp);
    return timestamp[node_id2] == 1;
}

static _Bool is_a_cut_node_in_UDGraph(const struct UDGraph_info *UDGraph, int node_id)
{
    size_t sub_tree_num = 0;
    /* timestamp in the traversal to the whole undirected graph */
    int timestamp[NODE_NUM] = {-1};
    timestamp[node_id] = 0;
    struct adj_line *adj_line = UDGraph->adj[node_id];
    while (adj_line != NULL && sub_tree_num < 2)
    {
        int adj_id = (adj_line->i_node != node_id) ? adj_line->i_node : adj_line->j_node;
        if (timestamp[adj_id] == -1)
        {
            Tarjan_algorithm_from_a_node_in_UDGraph(UDGraph, adj_id, 1, timestamp);
            sub_tree_num++;
        }
        adj_line = (adj_line->i_node == node_id) ? adj_line->i_next : adj_line->j_next;
    }
    return sub_tree_num >= 2;
}

/* a node in undirected tree */
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
        size_t left = 0, right = node->child_num - 1, middle;
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
    node->child_num++;
    node->next[pos] = new_leaf;
    new_leaf->parent = node;
    new_leaf->parent_id = node->node_id;
    return pos;
}

static void delete_all_nodes_in_undirc_tree(struct tree_node *node)
{
    if (node->child_num == 0)
    {
        free(node); return;
    }
    for (size_t i = 0; i < node->child_num; i++)
        delete_all_nodes_in_undirc_tree(node->next[i]);
    free(node);
    return;
}

static int find_disjt_root(int *disjt_set, int node_id)
{
    return node_id == disjt_set[node_id] ? node_id : (disjt_set[node_id] = find_disjt_root(disjt_set, node_id));
}

int latest_common_ancestor_in_undirc_tree(struct tree_node *node, int *disjt_set, _Bool *isvisited, int node_id,...)
{
    disjt_set[node->node_id] = node->node_id;
    for (size_t i = 0; i < node->child_num; i++)
        latest_common_ancestor_in_undirc_tree(node->next[i], node_id1, node_id2, disjt_set);
    if ( isvisited[node_id1] && node->next[i]->node_id == node_id2 )
        return find_disjt_root(disjt_set, node_id1);
    else if ( isvisited[node_id2] && node->next[i]->node_id == node_id1 )
        return find_disjt_root(disjt_set, node_id2);
    isvisited[node->node_id] = 1;
    disjt_set[node->node_id] = disjt_set[node->parent_id];
    return -1;
}