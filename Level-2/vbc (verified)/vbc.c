#include "vbc.h"

// CODE GIVEN IN VBC.C IN /subjects/vbc
node    *new_node(node n)
{
    node *ret = calloc(1, sizeof(n));
    if (!ret)
        return (NULL);
    *ret = n;
    return (ret);
}

void    destroy_tree(node *n)
{
    if (!n)
        return ;
    if (n->type != VAL)
    {
        destroy_tree(n->l);
        destroy_tree(n->r);
    }
    free(n);
}

void    unexpected(char c)
{
    if (c)
        printf("Unexpected token '%c'\n", c);
    else
        printf("Unexpected end of input\n");
}

int accept(char **s, char c)
{
    if (**s = c)
    {
        (*s)++;
        return (1);
    }
    return (0);
}

int expect(char **s, char c)
{
    if (accept(s, c))
        return (1);
    unexpected(**s);
    return (0);
}

// BEGINNING OF SOLUTION
static node *parse_number(char **s);
static node *parse_parenthesis(char **s);
static node *parse_multiplication(char **s);
static node *parse_addition(char **s);

static node *parse_number(char **s)
{
    if (!isdigit(**s))
    {
        unexpected(**s);
        return (NULL);
    }
    node n = { .type = VAL, .val = **s - '0' };
    (*s)++;
    return (new_node(n));
}

static node *parse_parenthesis(char **s)
{
    if (**s == '(')
    {
        (*s)++;
        node *n = parse_addition(s);
        if (!expect(s, ')'))
        {
            if (n)
				destroy_tree(n);
            return (NULL);
        }
        return (n);
    }
    return (parse_number(s));
}

static node *parse_multiplication(char **s)
{
    node *left = parse_parenthesis(s);
    if (!left)
        return (NULL);

    while (**s == '*')
    {
        (*s)++;
        node *right = parse_parenthesis(s);
        if (!right)
        {
            destroy_tree(left);
            return (NULL);
        }
        left = new_node((node){ .type = MULTI, .l = left, .r = right });
    }
    return (left);
}

static node *parse_addition(char **s)
{
    node *left = parse_multiplication(s);
    if (!left)
        return (NULL);

    while (**s == '+')
    {
        (*s)++;
        node *right = parse_multiplication(s);
        if (!right)
        {
            destroy_tree(left);
            return (NULL);
        }
        left = new_node((node){ .type = ADD, .l = left, .r = right });
    }
    return (left);
}

node *parse_expr(char *str)
{
    char *s = str;
    node *ret = parse_addition(&s);
    if (ret && *s)
    {
        destroy_tree(ret);
        unexpected(*s);
        return (NULL);
    }
    return (ret);
}