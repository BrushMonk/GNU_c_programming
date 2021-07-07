#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <ctype.h>
#include <inttypes.h>
#include <stddef.h>

static int get_string(char **ptr_to_ptr)
{
    size_t i = 0;
    char max_input[BUFSIZ];
    if (fgets(max_input, BUFSIZ, stdin))
    {
        for (i = 0; max_input[i] != '\n' || max_input[i] != '\0' || i < BUFSIZ; i++);
        if (max_input[i] == '\n') max_input[i] = '\0';
        int len = strlen(max_input) + 1;
        if ( (*ptr_to_ptr = (char *)realloc(*ptr_to_ptr,
        (strlen(max_input) + 1) * sizeof(char)) ) == NULL)
        {
        /* because the spaces where pointers inside structure point are not allocated and initialized. */
            perror("fail to allocate this input string");
            exit(EXIT_FAILURE);
        }
        strcpy(*ptr_to_ptr, max_input);
        if (**ptr_to_ptr != '\0') return len;
        else return -1;
    }
    else return -1;
}

#define STACKSIZE 4096
static char oprtr_stack[STACKSIZE];
static _Atomic(ptrdiff_t) oprtr_stack_top = -1;
static char popup_oprtr(void)
{
    if (oprtr_stack_top == -1)
        perror("pop() on the empty op stack, abort"), exit(-1);
    return oprtr_stack[oprtr_stack_top--];
}
static void push_oprtr(char ch)
{
    if (oprtr_stack_top == STACKSIZE)
        perror("stack overflow"), exit(-1);
    oprtr_stack[++oprtr_stack_top] = ch;
}

struct postfix_op {
char *op;
struct postfix_op *next_op;};
static void push_postfix_op(struct postfix_op **postfix_not, char *str, unsigned str_len)
{
    if ( (*postfix_not)->op == NULL )
    {
        (*postfix_not)->op = (char *)realloc( (*postfix_not)->op, str_len + 1 );
        strcpy((*postfix_not)->op, str);
        (*postfix_not)->next_op = NULL;
        return;
    }
    struct postfix_op *new_op = (struct postfix_op *)malloc(sizeof(struct postfix_op));
    new_op->op = (char *)malloc(str_len + 1);
    strcpy(new_op->op, str);
    /* use head insertion to creat a linked list */
    new_op->next_op = *postfix_not;
    *postfix_not = new_op;
}
void print_postfix_not(struct postfix_op *postfix_head)
{
    if (postfix_head->next_op != NULL)
        print_postfix_not(postfix_head->next_op);
    printf("%s ", postfix_head->op);
    return;
}
static void free_postfix_not(struct postfix_op **postfix_not)
{
    struct postfix_op *tmp;
    for(struct postfix_op *start = *postfix_not; start != NULL; free(tmp))
        tmp = start, start = start->next_op;
    return;
}

static uint_fast8_t oprtr_prec(char ch)
{
    switch(ch)
    {
        case '+': return 4;
        case '-': return 4;
        case '*': return 3;
        case '%': return 3;
        case '/': return 3;
        default:
            fputs("wrong argument in operator precedence!\n", stderr);
            exit(EXIT_FAILURE);
    }
}

typedef enum _error_t {
    OPRTOR_STARTS = 1,
#define OPRTOR_STARTS OPRTOR_STARTS
    OPRTOR_REPEATS = 2,
#define OPRTOR_REPEATS OPRTOR_REPEATS
    RIGHT_PARENTHESE_STARTS = 3,
#define RIGHT_PARENTHESE_STARTS RIGHT_PARENTHESE_STARTS
    NON_FIG_AND_OPRTOR = 4,
#define NON_FIG_AND_OPRTOR NON_FIG_AND_OPRTOR
    NO_RIGHT_PARENTHESE = 5,
#define NO_RIGHT_PARENTHESE NO_RIGHT_PARENTHESE
} _error_type;

int output_error_type(_error_type error_type, char *error)
{
    switch(error_type) {
    case OPRTOR_STARTS:
        fprintf(stderr, "error %d: %s", OPRTOR_STARTS, error);
        return OPRTOR_STARTS;
    case OPRTOR_REPEATS:
        fprintf(stderr, "error %d: %s", OPRTOR_REPEATS, error);
        return OPRTOR_REPEATS;
    case RIGHT_PARENTHESE_STARTS:
        fprintf(stderr, "error %d: %s", RIGHT_PARENTHESE_STARTS, error);
        return RIGHT_PARENTHESE_STARTS;
    case NON_FIG_AND_OPRTOR:
        fprintf(stderr, "error %d: %s", NON_FIG_AND_OPRTOR, error);
        return NON_FIG_AND_OPRTOR;
    case NO_RIGHT_PARENTHESE:
        fprintf(stderr, "error %d: %s", NO_RIGHT_PARENTHESE, error);
        return NO_RIGHT_PARENTHESE;
    default: return 0;
    }
}
#define output_error_type(error) output_error_type(error, #error)

int infix_to_postfix(char *infix_not, size_t infix_len, struct postfix_op **postfix_not)
{
    char num_str[infix_len];
    int8_t num_str_top = -1;
    _Bool is_reptd_oprtr = 0;
    if (infix_not[0] == '+' || infix_not[0] == '*' || infix_not[0] == '/' || infix_not[0] == ')' || infix_not[0] == '%')
        return output_error_type(OPRTOR_STARTS);
    for (size_t i = 0; infix_not[i] != '\0'; i++)
    {
        if (isblank(infix_not[i])) continue;
        else if (infix_not[i] == '+' || infix_not[i] == '-' || infix_not[i] == '*' || infix_not[i] == '/' || infix_not[i] == '%')
        {
            if (is_reptd_oprtr)
                return output_error_type(OPRTOR_REPEATS);
            while ( oprtr_stack_top != -1 && oprtr_prec(infix_not[i]) >= oprtr_prec(oprtr_stack[oprtr_stack_top]) )
                /* compound literal (char[]){popup_oprtr(), '\0'}
                is an unnamed string with only one char element. */
                push_postfix_op(postfix_not, (char[]){popup_oprtr(), '\0'}, 1);
            push_oprtr(infix_not[i]);
            is_reptd_oprtr = 1;
        }
        else if (infix_not[i] == '(') push_oprtr(infix_not[i]);
        else if (infix_not[i] == ')')
        {
            while (oprtr_stack_top != -1 && oprtr_stack[oprtr_stack_top] != '(')
                /* compound literal (char[]){popup_oprtr(), '\0'}
                is an unnamed string with only one char element. */
                push_postfix_op(postfix_not, (char[]){popup_oprtr(), '\0'}, 1);
            is_reptd_oprtr = 0;
            if (oprtr_stack_top == -1)
                return output_error_type(RIGHT_PARENTHESE_STARTS);
            else popup_oprtr();
        }
        else if ( isdigit(infix_not[i]) )
        {
            num_str[++num_str_top] = infix_not[i];
            is_reptd_oprtr = 0;
            if ( !isdigit(infix_not[i + 1]) || infix_not[i + 1] == '\0' )
            {
                push_postfix_op(postfix_not, num_str, strlen(num_str));
                memset(num_str, '\0', sizeof(num_str));
                num_str_top = -1;}
            }
        else return output_error_type(NON_FIG_AND_OPRTOR);
    }
    while (oprtr_stack_top != -1)
    {
        if (oprtr_stack[oprtr_stack_top] == '(')
            return output_error_type(NO_RIGHT_PARENTHESE);
        else
            /* compound literal (char[]){popup_oprtr(), '\0'}
            is an unnamed string with only one char element. */
            push_postfix_op(postfix_not, (char[]){popup_oprtr(), '\0'}, 1);
    }
    return 0;
}

int main(void)
{
    char *infix_not = (char *)malloc(sizeof(char));
    struct postfix_op *postfix_not = (struct postfix_op *)malloc(sizeof(struct postfix_op));
    puts("please input an arithmetic expression in need to transfer:");
    int infix_len;
    while ( (infix_len = get_string(&infix_not)) < 0 || infix_to_postfix(infix_not, infix_len, &postfix_not))
    {
        fputs("don't input an invalid infix expression, "
        "please input a valid infix expression again"
        "(only includes + - * / % five operators): ", stderr);
        oprtr_stack_top = -1;
        free_postfix_not(&postfix_not);
        postfix_not = (struct postfix_op *)malloc(sizeof(struct postfix_op));
    }
    puts("here is the equivalent postfix expression:");
    print_postfix_not(postfix_not);
    free_postfix_not(&postfix_not);
    putchar('\n');
    return 0;
}