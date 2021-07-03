#ifndef __HANS_STACK_H
#define __HANS_STACK_H

#include <stdlib.h>

struct _stack
{
  void *value;
  struct _stack *next;
};

typedef struct _stack Stack;

extern inline int empty(const Stack *stack)
{
  return stack == NULL;
}

extern inline void *top(const Stack *stack)
{
  if (empty(stack))
  {
    return NULL;
  }
  else
  {
    return stack->value;
  }
}

extern inline void *pop(Stack **stack)
{
  if (empty(*stack))
    return NULL;
  Stack *tmp = *stack;

  void *value = tmp->value;
  *stack = tmp->next;

  free(tmp);
  return value;
}

extern inline void push(Stack **in_stack, void *value)
{
  Stack *tmp = malloc(sizeof(Stack));
  tmp->value = value;
  tmp->next = *in_stack;
  *in_stack = tmp;
}

extern inline void purge(Stack **stack)
{
  Stack *tmp;

  while (!empty(*stack))
  {
    tmp = *stack;
    *stack = tmp->next;
    free(tmp);
  }
}

#endif