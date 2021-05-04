#pragma once
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define NODE_NUM 1024
/* adjacency multilist line */
struct adj_multiline
{   int i_node, j_node;
    struct adj_multiline *i_next, *j_next;
    int64_t weight;
    _Bool ismarked;};

/* weighted undirected graph infomation */
struct UDGraph_info
{
    /* the closest adjacency node */
    struct adj_multiline **closest_adj;
    size_t *degree;
    size_t line_num;
};

struct undirc_line
{   int i_node, j_node;
    int64_t weight;};

static void delete_all_lines_in_UDGraph(struct UDGraph_info *UDGraph)
{
    for (size_t v = 0; v < NODE_NUM; v++)
    {
        struct adj_multiline *cur = UDGraph->closest_adj[v];
        while (cur != NULL)
        {
            struct adj_multiline *tmp = cur;
            if (cur->i_node == v)
            {
                if (UDGraph->closest_adj[cur->j_node] == cur)
                    UDGraph->closest_adj[cur->j_node] = UDGraph->closest_adj[cur->j_node]->j_next;
                cur = cur->i_next;
            }
            if (cur->j_node == v)
            {
                if (UDGraph->closest_adj[cur->i_node] == cur)
                    UDGraph->closest_adj[cur->i_node] = UDGraph->closest_adj[cur->i_node]->i_next;
                cur = cur->j_next;
            }
            free(tmp);
        }
    }
    memset(UDGraph->closest_adj, 0, NODE_NUM * 8UL);
    memset(UDGraph->degree, 0, NODE_NUM * sizeof(size_t));
    UDGraph->line_num = 0;
    return;
}

static struct adj_multiline *insert_a_node_in_adj_multilist(struct adj_multiline *closest_adj, struct adj_multiline *new_node)
{
    struct adj_multiline *cur;
    *cur =  (struct adj_multiline){0};
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

static int add_a_undirc_line_in_UDGraph(struct UDGraph_info *UDGraph, struct undirc_line line)
{
    if (line.i_node >= NODE_NUM || line.j_node >= NODE_NUM || line.i_node < 0 || line.j_node < 0)
    {
        fputs("line node_id error. Fail to initialize undirected graph!\n", stderr);
        delete_all_lines_in_UDGraph(UDGraph);
        UDGraph = NULL;
        return -1;
    }
    /* use weight-ascending order to creat an adjacency multilist */
    struct adj_multiline *new_node = (struct adj_multiline *)malloc(sizeof(struct adj_multiline));
    memset(new_node, 0, sizeof(struct adj_multiline));
    new_node->i_node = line.i_node;
    new_node->j_node = line.j_node;
    new_node->weight = line.weight;
    if (UDGraph->closest_adj[line.i_node] == NULL)
        UDGraph->closest_adj[line.i_node] = new_node;
    else UDGraph->closest_adj[line.i_node] = insert_a_node_in_adj_multilist(UDGraph->closest_adj[line.i_node], new_node);
    UDGraph->degree[line.i_node]++;
    if (UDGraph->closest_adj[line.j_node] == NULL)
        UDGraph->closest_adj[line.j_node] = new_node;
    else UDGraph->closest_adj[line.j_node] = insert_a_node_in_adj_multilist(UDGraph->closest_adj[line.j_node], new_node);
    UDGraph->degree[line.j_node]++;
    UDGraph->line_num++;
    return 0;
}

int init_UDGraph(struct UDGraph_info *UDGraph, struct undirc_line lines[], size_t line_num)
{
    UDGraph->degree = (size_t *)malloc(NODE_NUM * sizeof(size_t));
    UDGraph->closest_adj = (struct adj_multiline **)malloc(NODE_NUM * 8UL);
    for (size_t v = 0; v < NODE_NUM; v++)
        UDGraph->closest_adj[v] = NULL;
    for (size_t e = 0; e < line_num; e++)
    {
        if (add_a_undirc_line_in_UDGraph(UDGraph, lines[e]) == -1)
            return -1;
        else continue;
    }
    return 0;
}

int delete_a_undirc_line_in_UDGraph(struct UDGraph_info *UDGraph, struct undirc_line line)
{
    struct adj_multiline *cur, *last;
    cur = UDGraph->closest_adj[line.i_node]; last = NULL;
    while (cur != NULL)
    {
        if (cur->j_node == line.j_node && cur->weight == line.weight)
        {
            if (last != NULL && last->i_next == cur)
                last->i_next = cur->i_next;
            else if (last != NULL &&  last->j_next == cur)
                last->j_next = cur->i_next;
            else UDGraph->closest_adj[line.i_node] = cur->i_next;
            break;
        }
        if (cur->i_node == line.j_node && cur->weight == line.weight)
        {
            if (last != NULL && last->i_next == cur)
                last->i_next = cur->j_next;
            else if (last != NULL &&  last->j_next == cur)
                last->j_next = cur->j_next;
            else UDGraph->closest_adj[line.i_node] = cur->j_next;
            break;
        }
        last = cur;
        cur = (cur->i_node == line.i_node) ? cur->i_next : cur->j_next;
    }
    cur = UDGraph->closest_adj[line.j_node]; last = NULL;
    while (cur != NULL)
    {
        if (cur->j_node == line.i_node && cur->weight == line.weight)
        {
            if (last != NULL && last->i_next == cur)
                last->i_next = cur->i_next;
            else if (last != NULL &&  last->j_next == cur)
                last->j_next = cur->i_next;
            else UDGraph->closest_adj[line.j_node] = cur->i_next;
            break;
        }
        if (cur->i_node == line.i_node && cur->weight == line.weight)
        {
            if (last != NULL && last->i_next == cur)
                last->i_next = cur->j_next;
            else if (last != NULL &&  last->j_next == cur)
                last->j_next = cur->j_next;
            else UDGraph->closest_adj[line.j_node] = cur->j_next;
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
    size_t middle, left = 0, right = node->child_num - 1;
    while (left <= right)
    {
        middle = left + ((right - left) >> 1);
        if (node->next[middle]->dist == new_leaf->dist)
            break;
        else if (node->next[middle]->dist > new_leaf->dist)
            right = middle - 1;
        else left = middle + 1;
    }
    size_t pos = left > middle ? left : middle;
    if ((node->next = (struct tree_node **)realloc(node->next, (node->child_num + 1) * 8UL)) == NULL)
    {
        perror("fail to allocate array");
        exit(EXIT_FAILURE);
    }
    for (size_t i = pos; i < node->child_num; i++)
        node->next[i + 1] = node->next[i];
    node->next[pos] = new_leaf;
    new_leaf->parent = node;
    node->child_num++;
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

static struct tree_node *copy_to_undirc_shortest_list(struct tree_node *node)
{
    struct tree_node *list_node = (struct tree_node *)malloc(sizeof(struct tree_node));
    memset(list_node, 0, sizeof(struct tree_node));
    list_node->next = (struct tree_node **)malloc(8UL);
    list_node->child_num = 1;
    list_node->dist = node->dist;
    list_node->node_id = node->node_id;
    list_node->parent_id = node->parent_id;
    return list_node;
}