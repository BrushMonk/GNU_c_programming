#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdint.h>
static _Atomic(enum {left, right}) side;
struct bin_node
{   int key;
    struct bin_node *next[2];};

// *queue[INT16_MAX] is an array consist of pointers.
static struct bin_node* volatile queue[INT16_MAX];
static _Atomic(ptrdiff_t) front = 0, rear = 0;
static void enqueue(struct bin_node *node)
{
    if (front == (rear + 1) % INT16_MAX)
    {
        fputs("queue overflow\n", stderr);
        exit(-1);
    }
    queue[rear++] = node;
    rear %= INT16_MAX;
}
static struct bin_node* dequeue(void)
{
    if (front == rear)
    {
        fputs("dequeue on the empty queue, abort\n", stderr);
        exit(-1);
    }
    struct bin_node *element = queue[front];
    front++;
    front %= INT16_MAX;
    return element;
}

void level_order_trav_to_init_min_bin_heap(struct bin_node **min_bin_heap, int n)
{
    /* use the queue to implement a level order traversal */
    struct bin_node *cur;
    for (int i = 0; i < n; i++)
    {
        struct bin_node *new_node = (struct bin_node *)malloc(sizeof(struct bin_node));
        if (i == 0)
            *min_bin_heap = new_node;
        new_node->key = i;
        new_node->next[left] = new_node->next[right] = NULL;
        enqueue(new_node);
        if (front != rear && !cur->next[left] && !cur->next[right])
            cur = dequeue();
        if (!cur->next[left])
        {
            cur->next[left] = new_node;
            continue;
        }
        if (!cur->next[right])
        {
            cur->next[right] = new_node;
            continue;
        }
    }
    while (front != rear) dequeue();
}

void min_bin_heapify(int *arr, int len, int start)
{
    /* set current node as minimum node temporily */
    int cur = start;
    int min_child = (cur << 1) + 1;
    while (min_child < len)
    {
        if (min_child + 1 < len && arr[min_child] > arr[min_child + 1])
            min_child++;
        if (arr[cur] < arr[min_child]) return;
        else
        {
            int tmp = arr[cur]; arr[cur] = arr[min_child]; arr[min_child] = tmp;
            cur = min_child;
            min_child = 2 * cur + 1;
        }
    }
    return;
}

struct binomial_node
{   int key;
    int degree;
    struct binomial_node *left_child;
    struct binomial_node *parent;
    struct binomial_node *sibling;};

/* the complexity of lookup in binomial heap is O(n) */
struct binomial_node *lookup_a_key_in_binomial_heap(struct binomial_node *node, int unkown_key)
{
    struct binomial_node *match_node;
    if (node == NULL) return NULL;
    else
    {
        if (node->key == unkown_key) return node;
        else
        {
            if ( (match_node = lookup_a_key_in_binomial_heap(node->left_child, unkown_key)) != NULL )
                return match_node;
            if ( (match_node = lookup_a_key_in_binomial_heap(node->sibling, unkown_key))  != NULL )
                return match_node;
        }
        return NULL;
    }
}

static struct binomial_node *link_binomial_trees(struct binomial_node *front, struct binomial_node *rear, struct binomial_node **binomial_heap)
{
    if (front->key <= rear->key)
    {
        front->sibling = rear->sibling;
        rear->parent = front;
        rear->sibling = front->left_child;
        front->left_child = rear;
        front->degree++;
    }
    else
    {
        if (front == *binomial_heap) rear = *binomial_heap;
        front->parent = rear;
        front->sibling = rear->left_child;
        rear->left_child = front;
        rear->degree++;
        front = rear;
    }
    return front;
}

struct binomial_node** init_binomial_heap(int *arr, int len)
{
    struct binomial_node **binomial_heap;
    *binomial_heap = NULL;
    for (int i = 0; i < len; i++)
    {
        struct binomial_node *new_node;
        *new_node = (struct binomial_node){0};
        new_node->key = arr[i];
        /* set new_node as the first binomial tree in binomial_heap */
        new_node->sibling = *binomial_heap;
        *binomial_heap = new_node;
        /* add new node in first of heap and unite binomial trees one by one */
        struct binomial_node *cur, *next_of_cur;
        for (cur = new_node; cur->sibling != NULL;)
        /* worst comlexity of inserting a node in binomial heap is O(log n) */
        {
            next_of_cur = cur->sibling;
            if (next_of_cur->sibling != NULL &&
            next_of_cur->degree == next_of_cur->sibling->degree) cur = cur->sibling;
            else if (cur->degree != next_of_cur->degree) break;
            else cur = link_binomial_trees(cur, next_of_cur, binomial_heap);
        }

    }
    return binomial_heap;
}

void delete_min_binomial_node(struct binomial_node **binomial_heap)
{
    struct binomial_node *min = *binomial_heap, *prev_of_min = NULL;
    struct binomial_node *i;
    *i = (struct binomial_node){0};
    for (i->sibling = *binomial_heap; i->sibling != NULL; i = i->sibling)
        if (i->sibling->key < min->key)
        {
            min = i->sibling;
            prev_of_min = i;
        }
    /* delete minimum-key node */
    if (prev_of_min == NULL)
        *binomial_heap = min->sibling;
    else prev_of_min->sibling = min->sibling;
    /* sort binomial trees in the degree of root */
    struct binomial_node *cur1 = *binomial_heap;
    struct binomial_node *cur2 = min->left_child;
    for (i = min->left_child; i != NULL; i = i->sibling)
        i->parent = NULL;
    /* delete minimum-key node */
    free(min);
    /* sort binomial trees in the degree of root */
    *binomial_heap = (cur1->degree < cur2->degree) ? cur1 : cur2;
    while (cur1 != NULL && cur2 != NULL)
    {
        if (cur1->degree < cur2->degree)
        {
            i = cur1;
            i = i->sibling;
            cur1 = cur1->sibling;
        }
        else
        {
            i = cur2;
            i = i->sibling;
            cur2 = cur2->sibling;
        }
    }
    while (cur1 != NULL)
    {
        i = cur1;
        i = i->sibling;
        cur1 = cur1->sibling;
    }
    while (cur2 != NULL)
    {
        i = cur2;
        i = i->sibling;
        cur2 = cur2->sibling;
    }
    /* merge binomial trees from head of binomial_heap */
    for (i = *binomial_heap; i->sibling != NULL;)
    {
        struct binomial_node *next_of_i = i->sibling;
        if ((i->degree != next_of_i->degree) ||
        ((next_of_i->sibling != NULL) && (next_of_i->degree == next_of_i->sibling->degree)))
            i = i->sibling;
        else i = link_binomial_trees(i, next_of_i, binomial_heap);
    }
    return;
}

int decrease_binomial_key(struct binomial_node **binomial_heap, int decreased_key, int new_key)
{
    if (decreased_key <= new_key)
    {
        fprintf(stderr, "decreased_key %d <=  new_key %d.\n", decreased_key, new_key);
        return -1;
    }
    struct binomial_node *decreased_node = lookup_a_key_in_binomial_heap(*binomial_heap, decreased_key);
    if (decreased_node == NULL)
    {
        fprintf(stderr, "No decreased_key %d in binomial_heap.\n", decreased_key);
        return -1;
    }
    decreased_node->key = new_key;
    for (struct binomial_node *cur = decreased_node;
    cur->parent != NULL && cur->key < cur->parent->key;
    cur = cur->parent)
    {
        int tmp = cur->key;
        cur->key = cur->parent->key;
        cur->parent->key = tmp;
    }
    return 0;
}

int delete_binomial_node(struct binomial_node **binomial_heap, int key_to_be_del)
{
    if ( decrease_binomial_key(binomial_heap, key_to_be_del, INT_MIN) < 0 )
        return -1;
    else
    {
        delete_min_binomial_node(binomial_heap);
        return 0;
    }
}