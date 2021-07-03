#ifndef __HANS_RBTREE_H
#define __HANS_RBTREE_H

#include <stdlib.h>
#include <stdbool.h>

typedef struct _rbtree_node
{
  int value;
  int color; // 0 for red, 1 for black.
  struct _rbtree_node *lnode;
  struct _rbtree_node *rnode;
} Node;

int rb_insert(Node **node, const int value_to_insert);
int rb_search(Node **node, const int value_to_find, Node *location);
int rb_delete(Node **node, const int value_to_delete);
void rb_print(const Node *node);

#endif