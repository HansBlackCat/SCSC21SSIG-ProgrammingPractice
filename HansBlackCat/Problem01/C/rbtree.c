#include <stdio.h>
#include "dbg.h"
#include "./stack.h"
#include "./rbtree.h"

#ifndef SUCESS
#define SUCCESS 1;
#endif
#ifndef EINVAL
#define EINVAL 22;
#endif

#ifdef DEBUG
#define ANSI_COLOR_RED "\x1b[31m"
#define ANSI_COLOR_BLACK "\x1b[30m"
#define ANSI_COLOR_RESET "\x1b[0m"
#endif
/*
typedef struct _rbtree_node
{
  int value;
  int color; // 0 for red, 1 for black.
  struct _rbtree_node *lnode;
  struct _rbtree_node *rnode;
} Node;
*/

static inline Node *create_rbtree_Node(int value_to_insert)
{
#ifdef DEBUG
  debug("Node Created with value: %d", value_to_insert);
#endif

  Node *node = malloc(sizeof(Node));
  node->value = value_to_insert;
  node->color = 0;
  node->lnode = NULL;
  node->rnode = NULL;
  return node;
}

static inline bool rb_is_empty(Node **node) { return *node == NULL ? true : false; }

void rb_print(const Node *node)
{
  if (node != NULL)
  {
    rb_print(node->lnode);
    printf(node->color == 0 ? ANSI_COLOR_RED "%d" ANSI_COLOR_RESET " " : ANSI_COLOR_BLACK "%d" ANSI_COLOR_RESET " ",
           node->value);
    rb_print(node->rnode);
  }
}

#ifdef DEBUG
void debug_rb_print(const Node *node)
{
  if (node != NULL)
  {
    debug_rb_print(node->lnode);
    fprintf(stderr, node->color == 0 ? ANSI_COLOR_RED "%d" ANSI_COLOR_RESET " " : ANSI_COLOR_BLACK "%d" ANSI_COLOR_RESET " ",
            node->value);
    debug_rb_print(node->rnode);
  }
}
#endif

int rb_insert(Node **node, const int value_to_insert)
{
#ifdef DEBUG
  debug("Insert to tree with value: %d", value_to_insert);
#endif

  Node **original_node = node;
  int left_right = -1, parent_left_right = -1;
  Node *new_node = create_rbtree_Node(value_to_insert);

  if (rb_is_empty(node))
  {
#ifdef DEBUG
    debug("Tree is now empty, insert new value: %d", value_to_insert);
#endif

    new_node->color = 1;
    *node = new_node;
    return SUCCESS;
  }

  Node *n = *node;
  Stack *stack = NULL;

  while (n != NULL)
  {
    if (value_to_insert == n->value)
    {
#ifdef DEBUG
      log_warn("Wrong inputs: Same value inputed");
#endif
      purge(&stack);
      return EINVAL;
    }

    push(&stack, n);
    if (value_to_insert < (n->value))
    {
      n = n->lnode;
    }
    else
    {
      n = n->rnode;
    }
  }

  Node *parent_node = top(stack);
  if ((new_node->value) < (parent_node->value))
  {
    parent_node->lnode = new_node;
    left_right = 1;
  }
  else
  {
    parent_node->rnode = new_node;
    left_right = 0;
  }

#ifdef DEBUG
  if (left_right == 0)
    debug("Insert %d, to the left side of %d", value_to_insert, parent_node->value);
  else if (left_right == 1)
    debug("Insert %d, to the right side of %d", value_to_insert, parent_node->value);
  else
    debug("Don't know where to go");
#endif

  // Restructuring & Recoloring
  Node *grandparent_node, *uncle_node;
  while (!empty(stack))
  {
    parent_node = pop(&stack);

    // Prevent from grandparents NULL check
    if (parent_node->color == 1)
      break;

    grandparent_node = pop(&stack);
    if (grandparent_node->lnode == parent_node)
    {
      uncle_node = grandparent_node->rnode;
      parent_left_right = 1;
    }
    else
    {
      uncle_node = grandparent_node->lnode;
      parent_left_right = 0;
    }

    // Restructuring
    if (uncle_node == NULL || uncle_node->color == 1)
    {
      if (left_right && parent_left_right)
      {
#ifdef DEBUG
        debug("  => lnode-lnode");
#endif
        grandparent_node->lnode = parent_node->rnode;
        parent_node->rnode = grandparent_node;
        parent_node->color = 1;
        grandparent_node->color = 0;

        if (!empty(stack))
        {
          Node *great_grand = top(stack);
          if (great_grand->lnode == grandparent_node)
          {
            great_grand->lnode = parent_node;
          }
          else
          {
            great_grand->rnode = parent_node;
          }
        }
        else
        {
          *node = parent_node;
        }
      }
      else if (left_right && !parent_left_right)
      {
#ifdef DEBUG
        debug("  => rnode-lnode");
#endif
        grandparent_node->rnode = new_node->lnode;
        parent_node->lnode = new_node->rnode;
        new_node->lnode = grandparent_node;
        new_node->rnode = parent_node;
        new_node->color = 1;
        grandparent_node->color = 0;

        if (!empty(stack))
        {
          Node *great_grand = top(stack);
          if (great_grand->lnode == grandparent_node)
          {
            great_grand->lnode = new_node;
          }
          else
          {
            great_grand->rnode = new_node;
          }
        }
        else
        {
          *node = new_node;
        }
      }
      else if (!left_right && parent_left_right)
      {
#ifdef DEBUG
        debug("  => lnode-rnode");
#endif
        grandparent_node->lnode = new_node->rnode;
        parent_node->rnode = new_node->lnode;
        new_node->lnode = parent_node;
        new_node->rnode = grandparent_node;
        new_node->color = 1;
        grandparent_node->color = 0;

        if (!empty(stack))
        {
          Node *great_grand = top(stack);
          if (great_grand->lnode == grandparent_node)
          {
            great_grand->lnode = new_node;
          }
          else
          {
            great_grand->rnode = new_node;
          }
        }
        else
        {
          *node = new_node;
        }
      }
      else // !left_right && !parent_left_right
      {
#ifdef DEBUG
        debug("  => rnode-rnode");
#endif
        grandparent_node->rnode = parent_node->lnode;
        parent_node->lnode = grandparent_node;
        parent_node->color = 1;
        grandparent_node->color = 0;

        if (!empty(stack))
        {
          Node *great_grand = top(stack);
          if (great_grand->lnode == grandparent_node)
          {
            great_grand->lnode = parent_node;
          }
          else
          {
            great_grand->rnode = parent_node;
          }
        }
        else
        {
          *node = parent_node;
        }
      }

      grandparent_node->color = 0;
      break; // Only one restructuring occurs
    }        // Resturcturing End

    // Recoloring
    if (uncle_node->color == 0)
    {
#ifdef DEBUG
      debug("  => recoloring from node-value: %d", new_node->value);
#endif
      grandparent_node->color = 0;
      parent_node->color = 1;
      uncle_node->color = 1;
      new_node = grandparent_node;
    } // Recoloring End
  }

  // Head check
  (*original_node)->color = 1;

  purge(&stack);

#ifdef DEBUG
  debug("Insert end");
  debug_rb_print(*original_node);
  fprintf(stderr, "\n");
#endif

  return SUCCESS;
}

int rb_search(Node **node, const int value_to_find, Node *location)
{
#ifdef DEBUG
  debug("Search value: %d", value_to_find);
#endif

  if (rb_is_empty(node))
  {
#ifdef DEBUG
    log_warn("Tree is empty");
#endif
    return EINVAL;
  }

  Node *current_node = *node;
  while (current_node->value != value_to_find || (current_node->lnode == NULL && current_node->rnode == NULL))
  {
    if (current_node->value < value_to_find)
    {
      current_node = current_node->rnode;
    }
    else
    {
      current_node = current_node->lnode;
    }
  }

  if (current_node->value != value_to_find)
  {
#ifdef DEBUG
    debug("No finding value");
#endif
    return EINVAL;
  }

  location = current_node;

#ifdef DEBUG
  debug("Search end");
#endif

  return SUCCESS;
}

int rb_delete(Node **node, const int value_to_delete)
{
#ifdef DEBUG
  debug("Delete value: %d", value_to_delete);
#endif

  //   Node **original_node = node;
  //   int left_right = -1, parent_left_right = -1;
  //   Stack *stack = NULL;

  //   Node *current_node = *node;
  //   while (value_to_delete != current_node->value && current_node != NULL)
  //   {
  //     push(&stack, current_node);
  //     current_node = value_to_delete < current_node->value ? current_node->lnode : current_node->rnode;
  //   }

  //   if (rb_is_empty(node))
  //   {
  // #ifdef DEBUG
  //     debug("Tree is empty");
  // #endif
  //     return EINVAL;
  //   }

  //   if (current_node->value != value_to_delete)
  //   {
  // #ifdef DEBUG
  //     debug("Tree don't have finding value: %d", value_to_delete);
  // #endif
  //     return EINVAL;
  //   }

  //   Node *parent_node = top(stack);

  //   if ((new_node->value) < (parent_node->value))
  //   {
  //     parent_node->lnode = new_node;
  //     left_right = 1;
  //   }
  //   else
  //   {
  //     parent_node->rnode = new_node;
  //     left_right = 0;
  //   }

  // #ifdef DEBUG
  //   if (left_right == 0)
  //     debug("Insert %d, to the left side of %d", value_to_delete, parent_node->value);
  //   else if (left_right == 1)
  //     debug("Insert %d, to the right side of %d", value_to_delete, parent_node->value);
  //   else
  //     debug("Don't know where to go");
  // #endif

  //   // Restructuring & Recoloring
  //   Node *grandparent_node, *uncle_node;
  //   while (!empty(stack))
  //   {
  //     parent_node = pop(&stack);

  //     // Prevent from grandparents NULL check
  //     if (parent_node->color == 1)
  //       break;

  //     grandparent_node = pop(&stack);
  //     if (grandparent_node->lnode == parent_node)
  //     {
  //       uncle_node = grandparent_node->rnode;
  //       parent_left_right = 1;
  //     }
  //     else
  //     {
  //       uncle_node = grandparent_node->lnode;
  //       parent_left_right = 0;
  //     }

  //     // Restructuring
  //     if (uncle_node == NULL || uncle_node->color == 1)
  //     {
  //       if (left_right && parent_left_right)
  //       {
  // #ifdef DEBUG
  //         debug("  => lnode-lnode");
  // #endif
  //         grandparent_node->lnode = parent_node->rnode;
  //         parent_node->rnode = grandparent_node;
  //         parent_node->color = 1;
  //         grandparent_node->color = 0;

  //         if (!empty(stack))
  //         {
  //           Node *great_grand = top(stack);
  //           if (great_grand->lnode == grandparent_node)
  //           {
  //             great_grand->lnode = parent_node;
  //           }
  //           else
  //           {
  //             great_grand->rnode = parent_node;
  //           }
  //         }
  //         else
  //         {
  //           *node = parent_node;
  //         }
  //       }
  //       else if (left_right && !parent_left_right)
  //       {
  // #ifdef DEBUG
  //         debug("  => rnode-lnode");
  // #endif
  //         grandparent_node->rnode = new_node->lnode;
  //         parent_node->lnode = new_node->rnode;
  //         new_node->lnode = grandparent_node;
  //         new_node->rnode = parent_node;
  //         new_node->color = 1;
  //         grandparent_node->color = 0;

  //         if (!empty(stack))
  //         {
  //           Node *great_grand = top(stack);
  //           if (great_grand->lnode == grandparent_node)
  //           {
  //             great_grand->lnode = new_node;
  //           }
  //           else
  //           {
  //             great_grand->rnode = new_node;
  //           }
  //         }
  //         else
  //         {
  //           *node = new_node;
  //         }
  //       }
  //       else if (!left_right && parent_left_right)
  //       {
  // #ifdef DEBUG
  //         debug("  => lnode-rnode");
  // #endif
  //         grandparent_node->lnode = new_node->rnode;
  //         parent_node->rnode = new_node->lnode;
  //         new_node->lnode = parent_node;
  //         new_node->rnode = grandparent_node;
  //         new_node->color = 1;
  //         grandparent_node->color = 0;

  //         if (!empty(stack))
  //         {
  //           Node *great_grand = top(stack);
  //           if (great_grand->lnode == grandparent_node)
  //           {
  //             great_grand->lnode = new_node;
  //           }
  //           else
  //           {
  //             great_grand->rnode = new_node;
  //           }
  //         }
  //         else
  //         {
  //           *node = new_node;
  //         }
  //       }
  //       else // !left_right && !parent_left_right
  //       {
  // #ifdef DEBUG
  //         debug("  => rnode-rnode");
  // #endif
  //         grandparent_node->rnode = parent_node->lnode;
  //         parent_node->lnode = grandparent_node;
  //         parent_node->color = 1;
  //         grandparent_node->color = 0;

  //         if (!empty(stack))
  //         {
  //           Node *great_grand = top(stack);
  //           if (great_grand->lnode == grandparent_node)
  //           {
  //             great_grand->lnode = parent_node;
  //           }
  //           else
  //           {
  //             great_grand->rnode = parent_node;
  //           }
  //         }
  //         else
  //         {
  //           *node = parent_node;
  //         }
  //       }

  //       grandparent_node->color = 0;
  //       break; // Only one restructuring occurs
  //     }        // Resturcturing End

  //     // Recoloring
  //     if (uncle_node->color == 0)
  //     {
  // #ifdef DEBUG
  //       debug("  => recoloring from node-value: %d", new_node->value);
  // #endif
  //       grandparent_node->color = 0;
  //       parent_node->color = 1;
  //       uncle_node->color = 1;
  //       new_node = grandparent_node;
  //     } // Recoloring End
  //   }

  //   // Head check
  //   (*original_node)->color = 1;

  //   purge(&stack);

  // #ifdef DEBUG
  //   debug("Delete ended");
  //   debug_rb_print(*original_node);
  //   fprintf(stderr, "\n");
  // #endif

  return SUCCESS;
}