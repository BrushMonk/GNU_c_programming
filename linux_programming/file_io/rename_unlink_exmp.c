#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

static char* get_string(void)
{
    size_t i;
    char max_input[BUFSIZ];
    if (fgets(max_input, BUFSIZ, stdin))
    {
        for (i = 0; max_input[i] != '\n' || max_input[i] != '\0' || i < BUFSIZ; i++);
        if (max_input[i] == '\n') max_input[i] = '\0';
        char *string=(char *)malloc(i * sizeof(char));
        /* allocated variable is located in heap area,
        which won't be released after function done. */
        strcpy(string, max_input);
        if (string[0] != '\0') return string;
        else return NULL;
    }
    else return NULL;
}

int main(void)
{
    FILE* fp = tmpfile();
    char *file_path = (char *)malloc(1024 * sizeof(char));
    /* copies an absolute pathname of the current working directory
    to the string new_file_name with len characters */
    if (getcwd(file_path, 1024 * sizeof(char)) == NULL)
        perror("getcwd error");
    printf("Please enter the new name of file %s:\n", file_path);
    char *new_file_name = get_string();
    if (rename(file_path, new_file_name) < 0)
        perror("rename error");
    if (unlink(file_path) < 0)
    {
        perror("unlink error");
        exit(EXIT_FAILURE);
    }
    if (remove(file_path) < 0)
    {
        perror("remove error");
        exit(EXIT_FAILURE);
    }
    exit(EXIT_SUCCESS);
}