#pragma once
#include "UDGraph.c"
/* x node subset from bipartite graph */
static int *volatile nodex;
static _Atomic(size_t) x_num = 0;
/* y node subset from bipartite graph */
static int *volatile nodey;
static _Atomic(size_t) y_num = 0;

static int color_nodes_from_a_node_in_UDGraph(const struct UDGraph_info *UDGraph,
int node_id, _Bool init_color, int color_set[])
{
    color_set[node_id] = init_color;
    struct adj_line *adj_line = UDGraph->adj[node_id];
    while (adj_line != NULL)
    {
        int adj_id = (adj_line->i_node != node_id) ? adj_line->i_node : adj_line->j_node;
        int unmatched_id = -1;
        if (color_set[adj_id] == -1)
        {
            unmatched_id = color_nodes_from_a_node_in_UDGraph(UDGraph, adj_id, !init_color, color_set);
            if (unmatched_id != -1) return unmatched_id;
        }
        else if (color_set[adj_id] != init_color)
            return adj_id;
        adj_line = (adj_line->i_node == node_id) ? adj_line->i_next : adj_line->j_next;
    }
    return -1;
}

/* if the undirected graph is bipartite, return -1.
or else return the node id who ocurs in odd cycle first. */
static int judge_bipartite(const struct UDGraph_info *UDGraph)
{
    int color_set[NODE_NUM] = {-1};
    int unmatched_id = -1;
    for (int v = 0; v < NODE_NUM; v++)
        if (color_set[v] == -1)
        {
            unmatched_id = color_nodes_from_a_node_in_UDGraph(UDGraph, v, 0, color_set);
            if (unmatched_id != -1) return unmatched_id;
        }
    for (int v = 0; v < NODE_NUM; v++)
    {
        if (color_set[v] == 0) x_num++;
        else if (color_set[v] == 1) y_num++;
    }
    nodex = (int *)malloc(x_num * sizeof(int));
    nodey = (int *)malloc(y_num * sizeof(int));
    for (int v = 0, xcount = 0, ycount = 0; v < NODE_NUM; v++)
    {
        if (color_set[v] == 0) nodex[xcount++] = v;
        else if (color_set[v] == 1) nodey[ycount++] = v;
    }
    return -1;
}

/* a matching in undircted graph */
struct matching
{   struct adj_line **matched_line;
    size_t line_num;
    int64_t weight_sum;};

static inline struct adj_line *get_matched_line(const struct UDGraph_info *UDGraph, int node_id)
{
    struct adj_line *adj_line = UDGraph->adj[node_id];
    while (adj_line != NULL && adj_line->ismarked == 0)
        adj_line = (adj_line->i_node == node_id) ? adj_line->i_next : adj_line->j_next;
    return adj_line;
}

static struct matching* get_all_matched_lines_in_UDGraph(const struct UDGraph_info *UDGraph, struct matching *__matching)
{
    _Bool isvisited[NODE_NUM] = {0};
    __matching->matched_line = (struct adj_line **)malloc(__matching->line_num * 8UL);
    for (size_t v = 0, e = 0; v < NODE_NUM; v++)
    {
        if (!isvisited[v])
        {
            struct adj_line *cur = UDGraph->adj[v];
            while (cur != NULL && cur->ismarked == 0)
                cur = cur->i_node == v ? cur->i_next : cur->j_next;
            if (cur != NULL)
            {
                __matching->matched_line[e++] = cur;
                __matching->weight_sum += cur->weight;
                cur->ismarked = 0;
                isvisited[cur->i_node] = isvisited[cur->j_node] = 1;
            }
        }
    }
    return __matching;
}

static _Bool update_augmenting_path_in_UWbipar(const struct UDGraph_info *UDGraph, int x_node, _Bool isvisited[])
{
    isvisited[x_node] = 1;
    struct adj_line *adj_line = UDGraph->adj[x_node];
    while (adj_line != NULL)
    {
        int y_node = (adj_line->i_node != x_node) ? adj_line->i_node : adj_line->j_node;
        /* visit those unvisited nodes */
        if (!isvisited[y_node])
        {
            isvisited[y_node] = 1;
            struct adj_line *y_match_line = get_matched_line(UDGraph, y_node);
            int y_match = -1;
            if (y_match_line != NULL)
                y_match = (y_match_line->i_node != y_node) ? y_match_line->i_node : y_match_line->j_node;
            /* find new match x node of y node node and recur itself until y node doesn't have match x node */
            if (y_match_line == NULL || update_augmenting_path_in_UWbipar(UDGraph, y_match, isvisited))
            {
                /* if y node has match x node, let match line unmarked */
                if (y_match_line != NULL)
                    y_match_line->ismarked = 0;
                /* let this unvisited x node become y's match node.
                if y node doesn't have match x node, let new x node be y's match node. */
                adj_line->ismarked = 1;
                return 1;
            }
        }
        /* if we can't find augmenting path from current y node, try another y node and repeat it. */
        adj_line = (adj_line->i_node == x_node) ? adj_line->i_next : adj_line->j_next;
    }
    /* if every adjacency node of x in alternating path is matched, we are unable to find a augmenting path.
    Therefore this path is the maximum alternating path from current x node.
    Then we jump out of this recursion. */
    return 0;
}

/* the worst complexity of Hungarian algorithm is O(n^2) */
struct matching *Hungarian_algorithm_in_UWbipar(const struct UDGraph_info *UDGraph)
{
    if (judge_bipartite(UDGraph) != -1)
    {
        fputs("The undirected graph is not bipartite.\n", stderr);
        return NULL;
    }
    struct matching *max_matching; *max_matching = (struct matching){0};
    _Bool isvisited[NODE_NUM] = {0};
    for (size_t xcount = 0; xcount < x_num; xcount++)
    {
        /* reset all nodes unvisited in UDGraph */
        memset(isvisited, 0, sizeof isvisited);
        max_matching->line_num += update_augmenting_path_in_UWbipar(UDGraph, nodex[xcount], isvisited);
    }
    x_num = 0; y_num = 0; free(nodex); free(nodey);
    max_matching = get_all_matched_lines_in_UDGraph(UDGraph, max_matching);
    return max_matching;
}

static int64_t *get_min_node_weight(const struct UDGraph_info *UDGraph)
{
    static int64_t node_weight[NODE_NUM] = {0};
    /* get minimum node weight */
    for (size_t xcount = 0; xcount < x_num; xcount++)
        if (UDGraph->adj[nodex[xcount]] != NULL)
            node_weight[nodex[xcount]] = UDGraph->adj[nodex[xcount]]->weight;
    return node_weight;
}

static _Bool update_min_augmenting_path_in_bipartite(const struct UDGraph_info *UDGraph,
int x_node, _Bool isvisited[], int64_t node_weight[], int64_t slack[])
{
    isvisited[x_node] = 1;
    struct adj_line *adj_line = UDGraph->adj[x_node];
    while (adj_line != NULL)
    {
        int y_node = (adj_line->i_node != x_node) ? adj_line->i_node : adj_line->j_node;
        /* visit those unvisited nodes */
        if (!isvisited[y_node])
        {
            /* if this line is in subGraph */
            if (node_weight[x_node] - node_weight[y_node] == adj_line->weight)
            {
                isvisited[y_node] = 1;
                struct adj_line *y_match_line = get_matched_line(UDGraph, y_node);
                int y_match = -1;
                if (y_match_line != NULL)
                    y_match = (y_match_line->i_node != y_node) ? y_match_line->i_node : y_match_line->j_node;
                /* find new match x node of y node node and recur itself until y node doesn't have match x node */
                if (y_match_line == NULL || update_min_augmenting_path_in_bipartite(UDGraph, y_match, isvisited, node_weight, slack))
                {
                    /* if y node has match x node, let match line unmarked */
                    if (y_match_line != NULL)
                        y_match_line->ismarked = 0;
                    /* let this unvisited x node become y's match node.
                    if y node doesn't have match x node, let new x node be y's match node. */
                    adj_line->ismarked = 1;
                    return 1;
                }
            }
            /* if this line is not in subGraph, record the minimum weight decrement for visited node */
            else if (adj_line->weight - node_weight[x_node] + node_weight[y_node] < slack[y_node])
                slack[y_node] = (adj_line->weight - node_weight[x_node] + node_weight[y_node]);
        }
        /* if we can't find augmenting path from current y node, try another y node and repeat it. */
        adj_line = (adj_line->i_node == x_node) ? adj_line->i_next : adj_line->j_next;
    }
    /* if every adjacency node of x in alternating path is matched, we are unable to find a augmenting path.
    Therefore this path is the maximum alternating path from current x node.
    Then we jump out of this recursion. */
    return 0;
}

/* the worst complexity of Kuhn Munkres algorithm is O(n^3) */
struct matching* min_Kuhn_Munkres_algorithm_in_bipartite(const struct UDGraph_info *UDGraph)
{
    if (judge_bipartite(UDGraph) != -1)
    {
        fputs("The undirected graph is not bipartite.\n", stderr);
        return NULL;
    }
    int64_t *node_weight = get_min_node_weight(UDGraph);
    struct matching *perf_matching; *perf_matching = (struct matching){0};
    _Bool isvisited[NODE_NUM] = {0};
    for (size_t xcount = 0; xcount < x_num; xcount++)
    {
        /* slack value used for increasing node weight */
        int64_t slack[NODE_NUM] = {INT64_MAX};
        while (1)
        {
            /* reset all nodes unvisited in UDGraph */
            memset(isvisited, 0, sizeof isvisited);
            if (update_min_augmenting_path_in_bipartite(UDGraph, nodex[xcount], isvisited, node_weight, slack))
            {
                perf_matching->line_num++;
                break;
            }
            int64_t min_slack = INT64_MAX;
            for (size_t ycount = 0; ycount < y_num; ycount++)
                if (!isvisited[nodey[ycount]] && min_slack > slack[nodey[ycount]])
                    /* search for minimum slack from unvisited y nodes */
                    min_slack = slack[nodey[ycount]];
            if (min_slack >= INT64_MAX) break;
            /* update slack */
            for (size_t i = 0; i < x_num; i++)
                if (isvisited[nodex[i]])
                    /* increase visited x node weight by minimum slack value */
                    node_weight[nodex[i]] += min_slack;
            for (size_t j = 0; j < y_num; j++)
                isvisited[nodey[j]] ?
                /* increase all visited y node weight by minimum slack value */
                (node_weight[nodey[j]] += min_slack):
                /* decrease all unvisited y nodes by minimum slack value */
                (slack[nodey[j]] -= min_slack);
        }
    }
    x_num = 0; y_num = 0; free(nodex); free(nodey);
    perf_matching = get_all_matched_lines_in_UDGraph(UDGraph, perf_matching);
    return perf_matching;
}

static int64_t* get_max_node_weight(const struct UDGraph_info *UDGraph)
{
    static int64_t node_weight[NODE_NUM] = {0};
    /* get maximum node weight */
    for (size_t xcount = 0; xcount < x_num; xcount++)
    {
        struct adj_line *cur = UDGraph->adj[nodex[xcount]], *last;
        while (cur != NULL)
        {
            last = cur;
            cur = (cur->i_node == nodex[xcount]) ? cur->i_next : cur->j_next;
        }
        if (last != NULL)
            node_weight[nodex[xcount]] = last->weight;
    }
    return node_weight;
}

static _Bool update_max_augmenting_path_in_bipartite(const struct UDGraph_info *UDGraph,
int x_node, _Bool isvisited[], int64_t node_weight[], int64_t slack[])
{
    isvisited[x_node] = 1;
    struct adj_line *adj_line = UDGraph->adj[x_node];
    while (adj_line != NULL)
    {
        int y_node = (adj_line->i_node != x_node) ? adj_line->i_node : adj_line->j_node;
        /* visit those unvisited nodes */
        if (!isvisited[y_node])
        {
            /* if this line is in subGraph */
            if (node_weight[x_node] + node_weight[y_node] == adj_line->weight)
            {
                isvisited[y_node] = 1;
                struct adj_line *y_match_line = get_matched_line(UDGraph, y_node);
                int y_match = -1;
                if (y_match_line != NULL)
                    y_match = (y_match_line->i_node != y_node) ? y_match_line->i_node : y_match_line->j_node;
                /* find new match x node of y node node and recur itself until y node doesn't have match x node */
                if (y_match_line == NULL || update_max_augmenting_path_in_bipartite(UDGraph, y_match, isvisited, node_weight, slack))
                {
                    /* if y node has match x node, let match line unmarked */
                    if (y_match_line != NULL)
                        y_match_line->ismarked = 0;
                    /* let this unvisited x node become y's match node.
                    if y node doesn't have match x node, let new x node be y's match node. */
                    adj_line->ismarked = 1;
                    return 1;
                }
            }
            /* if this line is not in subGraph, record the minimum weight variation for visited node */
            else if (node_weight[x_node] + node_weight[y_node] - adj_line->weight < slack[y_node])
                slack[y_node] = (node_weight[x_node] + node_weight[y_node] - adj_line->weight);
        }
        /* if we can't find augmenting path from current y node, try another y node and repeat it. */
        adj_line = (adj_line->i_node == x_node) ? adj_line->i_next : adj_line->j_next;
    }
    /* if every adjacency node of x in alternating path is matched, we are unable to find a augmenting path.
    Therefore this path is the maximum alternating path from current x node.
    Then we jump out of this recursion. */
    return 0;
}

/* the worst complexity of Kuhn Munkres algorithm is O(n^3) */
struct matching* max_Kuhn_Munkres_algorithm_in_bipartite(const struct UDGraph_info *UDGraph)
{
    if (judge_bipartite(UDGraph) != -1)
    {
        fputs("The undirected graph is not bipartite.\n", stderr);
        return NULL;
    }
    int64_t *node_weight = get_max_node_weight(UDGraph);
    struct matching *perf_matching; *perf_matching = (struct matching){0};
    _Bool isvisited[NODE_NUM] = {0};
    for (size_t xcount = 0; xcount < x_num; xcount++)
    {
        /* slack value used for variating node weight */
        int64_t slack[NODE_NUM] = {INT64_MAX};
        while (1)
        {
            /* reset all nodes unvisited in UDGraph */
            memset(isvisited, 0, sizeof(isvisited));
            if (update_max_augmenting_path_in_bipartite(UDGraph, nodex[xcount], isvisited, node_weight, slack))
            {
                perf_matching->line_num++;
                break;
            }
            int64_t min_slack = INT64_MAX;
            for (size_t ycount = 0; ycount < y_num; ycount++)
                if (!isvisited[nodey[ycount]] && min_slack > slack[nodey[ycount]])
                    /* search for minimum slack from unvisited y nodes */
                    min_slack = slack[nodey[ycount]];
            if (min_slack >= INT64_MAX) break;
            /* update slack */
            for (size_t i = 0; i < x_num; i++)
                if (isvisited[nodex[i]])
                    /* decrease visited x node weight by minimum slack value */
                    node_weight[nodex[i]] -= min_slack;
            for (size_t j = 0; j < y_num; j++)
                isvisited[nodey[j]] ?
                /* increase visited y node weight by minimum slack value */
                (node_weight[nodey[j]] += min_slack):
                /* decrease all unvisited y nodes by minimum slack value */
                (slack[nodey[j]] -= min_slack);
        }
    }
    x_num = 0; y_num = 0; free(nodex); free(nodey);
    perf_matching = get_all_matched_lines_in_UDGraph(UDGraph, perf_matching);
    return perf_matching;
}

/*

static _Atomic(size_t) odd_cycle_num[NODE_NUM] = {1};
int *volatile odd_cycle[NODE_NUM] = {NULL};
#define __init_odd_cycle(odd_cycle, n) \
    for (int v = 0; v < n; v++){ \
    odd_cycle[v] = (int *)malloc(8UL); \
    odd_cycle[v][0] = v;}

static int volatile queue[NODE_NUM];
static _Atomic(int) front = 0, rear = 0;
static void enqueue(int x_node)
{
    if (front == (rear + 1) % NODE_NUM)
        fputs("queue overflow\n", stderr), exit(-1);
    queue[rear++] = x_node;
    rear %= NODE_NUM;
    return;
}
static void enqueue_nodes_in_odd_cycle(int x_node)
{
    if (x_node < NODE_NUM)
        enqueue(x_node);
    else for (size_t i = 0; i < odd_cycle_num[x_node]; i++)
        enqueue_nodes_in_odd_cycle(odd_cycle[x_node][i]);
    return;
}
static int dequeue(void)
{
    if (front == rear)
    {
        fputs("dequeue on the empty queue, abort\n", stderr);
        exit(-1);
    }
    int node_id = queue[front];
    front++;
    front %= NODE_NUM;
    return node_id;
}

static inline int get_latest_common_ancestors(int node_id1, int node_id2,
int timestamp[], int disjt_set[], int spouse[], int pre_nodeid[])
{
    static int init_time = -1; ++init_time;
    while (1)
    {
        if (node_id1 != -1)
        {
            if (timestamp[node_id1] == init_time)
                return node_id1;
            timestamp[node_id1] = init_time;
            node_id1 = disjt_set[spouse[node_id1]];
            if (node_id1 == -1)
                node_id1 = disjt_set[pre_nodeid[node_id1]]];
        }
        int tmp_id = node_id1;
        node_id1 = node_id2, node_id2 = tmp_id;
    }
    return -1;
}

_Bool color_nodes_or_contract_or_augmenting_path_for_a_line(struct adj_line *line,
int disjt_set[], int spouse[], int color_set[], int pre_nodeid[], int64_t slack[])
{
    static int timestamp[NODE_NUM] = {-1};
    if (color_set[disjt_set[line->j_node]] == -1)
    {
        pre_nodeid[disjt_set[line->j_node]] = line->i_node; color_set[disjt_set[line->j_node]] = 1;
        color_set[disjt_set[spouse[disjt_set[line->j_node]]]] = 0;
        slack[disjt_set[line->j_node]] = slack[disjt_set[spouse[disjt_set[line->j_node]]]] = 0;
        enqueue_nodes_in_odd_cycle(disjt_set[spouse[disjt_set[line->j_node]]]);
    }
    else if (color_set[disjt_set[line->j_node]] == 0)
    {
    }
}

_Bool find_a_max_weight_matching_in_UDGraph(const struct UDGraph_info *UDGraph,
int node_id, int disjt_set[], int spouse[], int64_t node_weight[])
{
    int color_set[NODE_NUM] = {-1}; int64_t slack[NODE_NUM] = {INT64_MAX};
    int pre_nodeid[NODE_NUM] = {-1}; front = rear = 0;
    if (front == rear) return 0;
    enqueue(node_id);
    while (1)
    {
        while (front != rear)
        {
            if (color_set[disjt_set[queue[front]]] != 1)
            {
                struct adj_line *adj_line = UDGraph->adj[queue[front]];
                while (adj_line != NULL)
                {
                    int adj_id = (adj_line->i_node != queue[front]) ? adj_line->i_node : adj_line->j_node;
                    if (disjt_set[adj_id] != disjt_set[queue[front]])
                    {
                        if (node_weight[queue[front]] + node_weight[adj_id] != 2 * adj_line->weight)
                        {
                            if (!slack[disjt_set[adj_id]] || )
                                slack[disjt_set[adj_id]] = queue[front];
                        }
                        else if (color_nodes_or_contract_or_augmenting_path_for_a_line(adj_line, disjt_set, spouse, color_set, pre_nodeid, slack))
                            return 1;
                    }
                    adj_line = (adj_line->i_node == queue[front]) ? adj_line->i_next : adj_line->j_next;
                }
            }
            dequeue();
        }
    }
}

struct matching* max_blossom_algorithm_in_UDGraph(const struct UDGraph_info *UDGraph)
{
    int64_t max_weight;
    struct matching *perf_matching; *perf_matching = (struct matching){0};
    // the spouse node for every node id
    int spouse[NODE_NUM] = {-1};
    int disjt_set[NODE_NUM];
    __init_disjoint_set(disjt_set, NODE_NUM);
    __init_odd_cycle(odd_cycle, NODE_NUM);
    x_num = NODE_NUM;
    for (int v = 0; v < NODE_NUM; v++)
        if (spouse[v] == -1)
            find_a_max_weight_matching_in_UDGraph(UDGraph, v, disjt_set, spouse, node_weight);
    if (odd_cycle_num == 0)
    {
        fputs("The undirected graph doesn't have an odd cycle.\n", stderr);
        return max_Kuhn_Munkres_algorithm_in_bipartite(UDGraph);
    }
    else
    {
        return perf_matching;
    }
}

struct matching* min_blossom_algorithm_in_UDGraph(const struct UDGraph_info *UDGraph)
{
    int64_t *node_weight = get_min_node_weight(UDGraph);
}

*/