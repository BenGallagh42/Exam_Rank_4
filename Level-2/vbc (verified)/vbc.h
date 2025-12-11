// Code given in vbc.c file in /subjects/vbc/

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