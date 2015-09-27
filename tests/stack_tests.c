#include "minunit.h"
#include "stack.h"

static char* Test_GTStack_Init()
{
  GTStack s;
  GTStack_Init(&s, NULL, 10);
  mu_assert(s.ptr == 0, ".ptr not 0");
  mu_assert(s.top == NULL, ".top wrong");
  mu_assert(s.size == 10, ".size wrong");
  return NULL;
}

static char* Test_GTStack_IsEmpty()
{
  GTStack s;
  GTStackEntry e[1];
  GTStack_Init(&s, e, 1);
  mu_assert(GTStack_IsEmpty(&s), "empty stack not empty");
  int i = 0;
  GTStack_Push(&s, i);
  mu_assert(!GTStack_IsEmpty(&s), "non-empty stack is empty");
  return NULL;
}

static char* Test_GTStack_PushExplicit()
{
  GTStack s;
  GTStackEntry e[1];
  GTStack_Init(&s, e, 1);
  int i = -42;
  mu_assert(GTStack_PushExplicit(&s, i, &i) == 0, "stack overflow");
  mu_assert(e[0].val == i, ".val wrong");
  mu_assert(e[0].addr == &i, ".addr wrong");
  mu_assert(GTStack_PushExplicit(&s, 0, NULL) == -1, "undetected overflow");
  return NULL;
}

static char* Test_GTStack_Push()
{
  GTStack s;
  GTStackEntry e[1];
  GTStack_Init(&s, e, 1);
  int i = -36;
  mu_assert(GTStack_Push(&s, i) == 0, "stack overflow");
  mu_assert(e[0].val == i, ".val wrong");
  mu_assert(e[0].addr == &i, ".addr wrong");
  mu_assert(s.ptr == 1, ".ptr wrong");
  mu_assert(s.top == &e[1], ".top wrong");
  mu_assert(GTStack_Push(&s, i) == -1, "undetected overflow");
  return NULL;
}

static char* Test_GTStack_PushAndSetExplicit()
{
  GTStack s;
  GTStackEntry e[1];
  GTStack_Init(&s, e, 1);
  int i = 9999;
  const int j = -10;
  mu_assert(GTStack_PushAndSetExplicit(&s, i, &i, j) == 0, "stack overflow");
  mu_assert(e[0].val == 9999, ".val wrong");
  mu_assert(e[0].addr == &i, ".addr wrong");
  mu_assert(i == j, "wrong value set");
  mu_assert(s.ptr == 1, ".ptr wrong");
  mu_assert(s.top == &e[1], ".top wrong");
  mu_assert(GTStack_PushAndSetExplicit(&s, 0, NULL, 0) == -1,
   "undetected overflow");
  return NULL;
}

static char* Test_GTStack_PushAndSet()
{
  GTStack s;
  GTStackEntry e[1];
  GTStack_Init(&s, e, 1);
  int i = -93483;
  mu_assert(GTStack_PushAndSet(&s, i, 348) == 0, "stack overflow");
  mu_assert(e[0].val == -93483, ".val wrong");
  mu_assert(e[0].addr == &i, ".addr wrong");
  mu_assert(i == 348, "wrong value set"); 
  return NULL;
}

static char* Test_GTStack_Pop()
{
  GTStack s;
  GTStackEntry e[1];
  GTStack_Init(&s, e, 1);
  int i = 38;
  mu_assert(GTStack_Push(&s, i) == 0, "stack overflow");
  mu_assert(GTStack_Pop(&s) == 0, "stack underflow");
  mu_assert(s.ptr == 0, ".ptr wrong");
  mu_assert(s.top == e, ".top wrong");
  mu_assert(GTStack_Pop(&s) == -1, "undetected underflow");
  return NULL;
}

static char* Test_GTStack_Peek()
{
  GTStack s;
  GTStackEntry e[1];
  GTStackEntry f;
  GTStack_Init(&s, e, 1);
  int i = -60;
  mu_assert(GTStack_Push(&s, i) == 0, "stack overflow");
  mu_assert(GTStack_Peek(&s, &f) == 0, "empty stack");
  mu_assert(f.val == i, "wrong .val");
  mu_assert(f.addr == &i, "wrong .addr");
  mu_assert(GTStack_Pop(&s) == 0, "stack underflow");
  mu_assert(GTStack_Peek(&s, &f) == -1, "undetected empty stack");
  return NULL;
}

static char* Test_All()
{
  mu_suite_start();
  mu_run_test(Test_GTStack_Init);
  mu_run_test(Test_GTStack_IsEmpty);
  mu_run_test(Test_GTStack_PushExplicit);
  mu_run_test(Test_GTStack_Push);
  mu_run_test(Test_GTStack_PushAndSetExplicit);
  mu_run_test(Test_GTStack_PushAndSet);
  mu_run_test(Test_GTStack_Pop);
  mu_run_test(Test_GTStack_Peek);
  return NULL;
}

RUN_TESTS(Test_All)
