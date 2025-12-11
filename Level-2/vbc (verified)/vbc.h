#ifndef VBC_H
#define VBC_H

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

typedef struct node {
    enum {
        ADD,
        MULTI,
        VAL
    }   type;
    int val;
    struct node *l;
    struct node *r;
}   node;

node *new_node(node n);
void destroy_tree(node *n);
void unexpected(char c);
int accept(char **s, char c);
int expect(char **s, char c);
int eval_tree(node *tree);
node *parse_expr(char *s);
static node *parse_number(char **s);
static node *parse_parenthesis(char **s);
static node *parse_multiplication(char **s);
static node *parse_addition(char **s);

#endif