here are three ways to write a dynamic array input
(use space to seperate every number):
```cpp
#include <stdio.h>
#include <inttypes.h>
static int32_t* get_array(void)
{
    size_t len = 1;
    int32_t *array = (int32_t *)malloc(sizeof(int32_t));
    static char buf[BUFSIZ];
    void *alloc_check;
    fprintf(stdout, "Enter decimal int32_teger arrays(use spaces key to separate every number):\n");
    while ( fscanf(stdin, "%d", array + len - 1) == 1 )
    {
        if ( (array = (int32_t *)realloc(array, (len + 1) * sizeof(int32_t))) == NULL )
        {
            perror("fail to allocate then input array");
            exit(EXIT_FAILURE);
        }
        len++;
        if ( getchar() == '\n' ) break;
    }
    setbuf(stdin, NULL);
    setvbuf(stdin, buf, _IOLBF, BUFSIZ);
    /* skip rest of input content */
    return array;
}/* if you are sure you won't use array anymore, please must use free(array); */

static int32_t* get_array(void)
{
    size_t i;
    int32_t max_input[BUFSIZ];
    for (i = 0; i < BUFSIZ; i++);
        fscanf(stdin, "%d", max_input[i]);
    for (i = 0; max_input[i] != 0; i++);
    int32_t *array=(int32_t *)malloc(i * sizeof(int32_t));
    /* static variable is located in data segment area, which won't be released after function done. */
    for (i = 0; max_input[i] != 0; i++)
        array[i] = max_input[i];
    while ( getchar() != '\n' ) continue;/* skip rest of input content */
    /* skip rest of input content */
    return array;
}

static size_t get_array(int32_t **ptr_to_ptr)
{
    size_t len = 1;
    static char buf[BUFSIZ];
    void *alloc_check;
    fprintf(stdout, "Enter decimal int32_teger arrays(use spaces key to separate every number):\n");
    while ( fscanf(stdin, "%d", *ptr_to_ptr + len - 1) == 1 )
    {
        if ( (*ptr_to_ptr = (int32_t *)realloc( *ptr_to_ptr,
        (len + 1) * sizeof(int32_t) ) ) == NULL)
        {
            perror("fail to allocate the input array");
            exit(EXIT_FAILURE);
        }
        len++;
        if ( getchar() == '\n' ) break;
    }
    setbuf(stdin, NULL);
    setvbuf(stdin, buf, _IOLBF, BUFSIZ);
    /* skip rest of input content */
    return len;
}
```

here are two ways to write a dynamic string input:
```cpp
#include <stdio.h>
#include <string.h>
#include <inttypes.h>
static int get_string(char **ptr_to_ptr)
{
    size_t i;
    char max_input[BUFSIZ];
    if (fgets(max_input, BUFSIZ, stdin))
    {
        for (i = 0; max_input[i] != '\n' || max_input[i] != '\0' || i < BUFSIZ; i++);
        if (max_input[i] == '\n') max_input[i] = '\0';
        int len = strlen(max_input) + 1;
        if ( ( *ptr_to_ptr = (char *)realloc( *ptr_to_ptr,
        len * sizeof(char) )) == NULL )
        {
        /* because the spaces where point32_ters inside structure point32_t are not allocated and initialized. */
            perror("fail to allocate this input string");
            exit(EXIT_FAILURE);
        }
        strcpy(*ptr_to_ptr, max_input);
        if( **ptr_to_ptr != '\0' ) return len;
        else return -1;
    }
    else return -1;
}

static char* get_string(void)
{
    size_t i;
    char max_input[BUFSIZ];
    if ( fgets(max_input, BUFSIZ, stdin) )
    {
        for (i = 0; max_input[i] != '\n' || max_input[i] != '\0' || i < BUFSIZ; i++);
        if (max_input[i] == '\n') max_input[i] = '\0';
        char *string = (char *)malloc(i * sizeof(char));
        /* allocated variable is located in heap area, which won't be released after function done. */
        strcpy(string, max_input);
        if (string[0] != '\0') return string;
        else return NULL;
    }
    else return NULL;
}
```

here is a way to write a integer input without rest in buffer:
```cpp
static int64_t get_64bits_integer(void)
{
    int64_t number;
    _Bool input_check;
    fputs("Enter a number: ", stdout);
    while( (input_check = fscanf(stdin, "%ld", &number)) != 1)
    if (input_check != 1)
    {
        scanf("%*s");
        fprintf(stdout, "invalid input, enter this number again: ");
    }
    return number;
}
```