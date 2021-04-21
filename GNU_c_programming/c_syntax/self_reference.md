```cpp
struct list_node {
char *data;
struct list_node *next;};
int main(void)
{
    struct list_node *node = calloc(1, sizeof(struct list_node));
    node->data = SomeString;
    node->next = node; /* self reference */
    /* node=node->next; is not self reference. */
    return 0;
}
```