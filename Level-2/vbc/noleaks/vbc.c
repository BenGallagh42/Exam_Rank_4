#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

typedef struct node {
    enum {
        ADD,
        MULTI,
        VAL
    } type;
    int val;
    struct node *l;
    struct node *r;
} node;

node *new_node(node n)
{
    node *ret = calloc(1, sizeof(n));
    if (!ret)
        return (NULL);
    *ret = n;
    return (ret);
}

void destroy_tree(node *n)
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

void unexpected(char c)
{
    if (c)
        printf("Unexpected token '%c'\n", c);
    else
        printf("Unexpected end of input\n");
}

int accept(char **s, char c)
{
    if (**s == c)
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

/* ============= FONCTIONS INTERNES ============= */
static node *parse_main(char **s);
static node *parse_term(char **s);
static node *parse_factor(char **s);
static node *parse_number(char **s);
/* ============================================= */

static node *parse_main(char **s)
{
    node *left = parse_term(s);
    if (!left)
        return (NULL);
    while (**s == '+')
    {
        (*s)++;
        node *right = parse_term(s);
        if (!right)
        {
            destroy_tree(left);
            return (NULL);
        }
        node n = { .type = ADD, .l = left, .r = right };
        left = new_node(n);
        if (!left)
        {
            destroy_tree(left);
            destroy_tree(right);
            return (NULL);
        }
    }
    return (left);
}

static node *parse_term(char **s)
{
    node *left = parse_factor(s);
    if (!left)
        return (NULL);
    while (**s == '*')
    {
        (*s)++;
        node *right = parse_factor(s);
        if (!right)
        {
            destroy_tree(left);
            return (NULL);
        }
        node n = { .type = MULTI, .l = left, .r = right };
        left = new_node(n);
        if (!left)
        {
            destroy_tree(left);
            destroy_tree(right);
            return (NULL);
        }
    }
    return (left);
}

static node *parse_factor(char **s)
{
    if (**s == '(')
    {
        (*s)++;
        node *n = parse_main(s);
        if (!n || !expect(s, ')'))
        {
            if (n)
                destroy_tree(n);
            return (NULL);
        }
        return (n);
    }
    return (parse_number(s));
}

static node *parse_number(char **s)
{
    if (!isdigit(**s))
    {
        unexpected(**s);
        return (NULL);
    }
    int val = **s - '0';
    (*s)++;
    node n = { .type = VAL, .val = val };
    return (new_node(n));
}

node *parse_expr(char *s)
{
    char *ptr = s;
    node *ret = parse_main(&ptr);
    if (ret && *ptr)
    {
        destroy_tree(ret);
        unexpected(*ptr);
        return (NULL);
    }
    return (ret);
}