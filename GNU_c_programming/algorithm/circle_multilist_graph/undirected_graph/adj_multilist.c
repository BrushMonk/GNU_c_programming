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
    size_t *degree;
    size_t side_num;
};

struct undirc_side
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
    for (size_t v = 0; v < NODE_NUM; v++)
    {
        struct adj_multinode *cur = UDGraph->closest_adj[v];
        while (cur != NULL)
        {
            struct adj_multinode *tmp = cur;
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
    UDGraph->side_num = 0;
    return;
}

int init_UDGraph(struct UDGraph_info *UDGraph, struct undirc_side sides[], size_t side_num)
{
    UDGraph->degree = (size_t *)malloc(NODE_NUM * sizeof(size_t));
    UDGraph->closest_adj = (struct adj_multinode **)malloc(NODE_NUM * 8UL);
    for (size_t v = 0; v < NODE_NUM; v++)
        UDGraph->closest_adj[v] = NULL;
    for (size_t e = 0; e < side_num; e++)
    {
        if (sides[e].i_node >= NODE_NUM || sides[e].j_node >= NODE_NUM)
        {
            fputs("side node_id error. Fail to initialize undirected graph!\n", stderr);
            delete_all_sides_in_UDGraph(UDGraph);
            UDGraph = NULL;
            return -1;
        }
        /* use weight-ascending order to creat an adjacency multilist */
        struct adj_multinode *new_node = (struct adj_multinode *)malloc(sizeof(struct adj_multinode));
        memset(new_node, 0, sizeof(struct adj_multinode));
        new_node->i_node = sides[e].i_node;
        new_node->j_node = sides[e].j_node;
        new_node->weight = sides[e].weight;
        if (UDGraph->closest_adj[sides[e].i_node] == NULL)
            UDGraph->closest_adj[sides[e].i_node] = new_node;
        else UDGraph->closest_adj[sides[e].i_node] = insert_a_node_in_adj_multilist(UDGraph->closest_adj[sides[e].i_node], new_node);
        UDGraph->degree[sides[e].i_node]++;
        if (UDGraph->closest_adj[sides[e].j_node] == NULL)
            UDGraph->closest_adj[sides[e].j_node] = new_node;
        else UDGraph->closest_adj[sides[e].j_node] = insert_a_node_in_adj_multilist(UDGraph->closest_adj[sides[e].j_node], new_node);
        UDGraph->degree[sides[e].j_node]++;
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

/* a node in undirected tree */
struct undirc_tree_node
{   int node_id;
    int64_t dist;
    struct undirc_tree_node **next;
    int parent_id;
    struct undirc_tree_node *parent;
    size_t child_num;};

static size_t insert_leaf_in_undirc_tree_node(struct undirc_tree_node *node, struct undirc_tree_node *new_leaf)
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
    if ((node->next = (struct undirc_tree_node **)realloc(node->next, (node->child_num + 1) * 8UL)) == NULL)
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

static void delete_all_nodes_in_undirc_tree(struct undirc_tree_node *node)
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

static struct undirc_tree_node *copy_to_undirc_shortest_list(struct undirc_tree_node *node)
{
    struct undirc_tree_node *list_node = (struct undirc_tree_node *)malloc(sizeof(struct undirc_tree_node));
    memset(list_node, 0, sizeof(struct undirc_tree_node));
    list_node->next = (struct undirc_tree_node **)malloc(8UL);
    list_node->child_num = 1;
    list_node->dist = node->dist;
    list_node->node_id = node->node_id;
    list_node->parent_id = node->parent_id;
    return list_node;
}