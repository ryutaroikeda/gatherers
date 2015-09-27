#include "dbg.h"
#include "stack.h"

int GTStack_Init(GTStack* s, GTStackEntry* entries, int size)
{
  check(size > 0, "nonpositive size");
  s->size = size;
  s->ptr = 0;
  s->err = GTStackError_None;
  s->top = entries;
  return 0;
  error:
  return -1;
}

int GTStack_IsEmpty(const GTStack* s)
{
  return s->ptr <= 0;
}

int GTStack_PushExplicit(GTStack* s, int val, int* addr)
{
  check(s->ptr < s->size, "stack overflow");
  s->top->val = val;
  s->top->addr = addr;
  s->ptr++;
  s->top++;
  return 0;
  error:
  s->err = GTStackError_Overflow;
  return -1;
}

int GTStack_PushAndSetExplicit(GTStack* s, int val, int* addr, int set)
{
  check(s->ptr < s->size, "stack overflow");
  s->top->val = val;
  s->top->addr = addr;
  s->ptr++;
  s->top++;
  *addr = set;
  return 0;
  error:
  s->err = GTStackError_Overflow;
  return -1;
}

int GTStack_Pop(GTStack* s)
{
  check(s->ptr > 0, "stack underflow");
  s->ptr--;
  s->top--;
  *(s->top->addr) = s->top->val;
  return 0;
  error:
  s->err = GTStackError_Underflow;
  return -1;
}

int GTStack_Purge(GTStack* s)
{
  check(s->ptr > 0, "nothing to purge");
  s->ptr--;
  s->top--;
  return 0;
  error:
  s->err = GTStackError_EmptyPurge;
  return -1;
}

int GTStack_Peek(GTStack* s, GTStackEntry* e)
{
  check(s->ptr > 0, "nothing to peek");
  s->top--;
  e->val = s->top->val;
  e->addr = s->top->addr;
  s->top++;
  return 0;
  error:
  s->err = GTStackError_EmptyPeek;
  return -1;
}

int GTStack_BeginPlay(GTStack* s)
{
  GTStack_PushExplicit(s, GTStackCode_BeginPlay, NULL);
  return 0;
}

int GTStack_BeginTurn(GTStack* s)
{
  GTStack_PushExplicit(s, GTStackCode_BeginTurn, NULL);
  return 0;
}
