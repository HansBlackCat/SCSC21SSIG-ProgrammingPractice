#include <stdio.h>
#include "rbtree.h"
#include "dbg.h"

int main()
{
#ifdef DEBUG
  log_info("\n========= DEBUG MODE ON =========\n");
#endif

  Node *node = NULL;
  Node *location = NULL;
  rb_search(&node, 6, location);

  rb_insert(&node, 3);
  rb_insert(&node, 1);
  rb_insert(&node, 10);
  rb_insert(&node, 6);
  rb_insert(&node, 4);
  rb_insert(&node, 7);

  rb_search(&node, 6, location);

  printf("END\n");
}