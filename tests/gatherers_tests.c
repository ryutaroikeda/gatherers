#include "gatherers.h"
#include "minunit.h"

static char* Test_GTStack_Init()
{
  GTStack s;
  GTStack_Init(&s, NULL, 10);
  mu_assert(s.ptr == 0, ".ptr not 0");
  mu_assert(s.top == NULL, ".top wrong");
  mu_assert(s.size == 10, ".size wrong");
  return NULL;
}

static char* Test_GTStack_PushExplicit()
{
  GTStack s;
  GTStackEntry e[1];
  GTStack_Init(&s, e, 1);
  int i = -42;
  mu_assert(GTStack_PushExplicit(&s, i, NULL) == 0, "stack overflow");
  mu_assert(e[0].val == i, ".val wrong");
  mu_assert(e[0].addr == NULL, ".addr wrong");
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

static char* Test_GTBoard_IsValid()
{
  GTBoard b;
  GTBoard_Init(&b);
  mu_assert(GTBoard_IsValid(&b, GTBoard_ValidMin), "valid not valid");
  mu_assert(!GTBoard_IsValid(&b, GTBoard_InvalidMin), "invalid is valid");
  return NULL;
}

static char* Test_GTBoard_IsEmpty()
{
  GTBoard b;
  GTBoard_Init(&b);
  mu_assert(GTBoard_IsEmpty(&b, GTBoard_ValidMin), "empty not empty");
  mu_assert(GTBoard_CreateUnit(&b, GTPlayer_None, GTUnitType_None,
   GTBoard_ValidMin) == 0, "unit overflow");
  mu_assert(!GTBoard_IsEmpty(&b, GTBoard_ValidMin), "unit is empty");
  mu_assert(!GTBoard_IsEmpty(&b, GTBoard_InvalidMin), "invalid is empty");
  return NULL;
}

static char* Test_GTBoard_IsUnit()
{
  GTBoard b;
  GTBoard_Init(&b);
  mu_assert(!GTBoard_IsUnit(&b, GTBoard_ValidMin), "empty is unit");
  mu_assert(GTBoard_CreateUnit(&b, GTPlayer_None, GTUnitType_None,
   GTBoard_ValidMin) == 0, "unit overflow");
  mu_assert(GTBoard_IsUnit(&b, GTBoard_ValidMin), "unit not unit");
  mu_assert(!GTBoard_IsUnit(&b, GTBoard_InvalidMin), "invalid is unit");
  return NULL;
}

static char* Test_GTBoard_CreateUnit()
{
  GTBoard b;
  GTBoard_Init(&b);
  mu_assert(GTBoard_CreateUnit(&b, GTPlayer_Black, GTUnitType_Archer,
   GTBoard_ValidMin) == 0, "unit overflow");
  mu_assert(b.unitId == 1, ".unitId not updated");
  mu_assert(b.board[GTBoard_ValidMin] == 0, ".board not updated");
  mu_assert(b.units[0].type == GTUnitType_Archer, "created wrong unit");
  mu_assert(b.units[0].color == GTPlayer_Black, ".color wrong");
  int i;
  for(i = 0; i < GTBoard_UnitSize - 1; i++)
  {
    mu_assert(GTBoard_CreateUnit(&b, GTPlayer_Black, GTUnitType_Archer,
     GTBoard_ValidMin) == 0, "unit overflow");
  }
  mu_assert(GTBoard_CreateUnit(&b, GTPlayer_Black, GTUnitType_Archer,
   GTBoard_ValidMin) == -1, "undetected overflow");
  return NULL;
}

static char* Test_GTBoard_IsVisible()
{
  GTBoard b;
  GTBoard_Init(&b);
  mu_assert(!GTBoard_IsVisible(&b, GTBoard_ValidMin), "invisible is visible");
  b.tiles[GTBoard_ValidMin].isVisible = 1;
  mu_assert(GTBoard_IsVisible(&b, GTBoard_ValidMin), "visible is invisible");
  return NULL;
}

static char* Test_GTBoard_IsValidUnitId()
{
  GTBoard b;
  GTBoard_Init(&b);
  mu_assert(!GTBoard_IsValidUnitId(&b, 0), "invalid is valid");
  b.unitId++;
  mu_assert(GTBoard_IsValidUnitId(&b, 0), "valid is invalid");
  return NULL;
}

static char* Test_GTBoard_CanMoveUnit()
{
  return NULL;
}

static char* Test_GTBoard_RevealTile()
{
  GTBoard b;
  GTBoard_Init(&b);
  mu_assert(GTBoard_RevealTile(&b, GTBoard_ValidMin) == 0, "reveal failed");
  mu_assert(b.tiles[GTBoard_ValidMin].isVisible == 1, ".isVisible wrong");
  return NULL;  
}

static char* Test_GTBoard_ResetUnitMovement()
{
  GTBoard b;
  GTBoard_Init(&b);
  GTBoard_CreateUnit(&b, GTPlayer_None, GTUnitType_Cavalry, GTBoard_ValidMin);
  mu_assert(GTBoard_ResetUnitMovement(&b, 0) == 0, "reset failed");
  mu_assert(b.units[0].movement == 2, ".movement wrong");
  return NULL;
}

static char* Test_GTBoard_MoveUnit()
{
  return NULL;
}

static char* Test_All()
{
  mu_suite_start();
  mu_run_test(Test_GTStack_Init);
  mu_run_test(Test_GTStack_PushExplicit);
  mu_run_test(Test_GTStack_Push);
  mu_run_test(Test_GTStack_Pop);
  mu_run_test(Test_GTStack_Peek);
  mu_run_test(Test_GTBoard_IsValid);
  mu_run_test(Test_GTBoard_IsEmpty);
  mu_run_test(Test_GTBoard_IsUnit);
  mu_run_test(Test_GTBoard_IsVisible);
  mu_run_test(Test_GTBoard_IsValidUnitId);
  mu_run_test(Test_GTBoard_CanMoveUnit);
  mu_run_test(Test_GTBoard_RevealTile);
  mu_run_test(Test_GTBoard_CreateUnit);
  mu_run_test(Test_GTBoard_ResetUnitMovement);
  mu_run_test(Test_GTBoard_MoveUnit);
  return NULL;
}

RUN_TESTS(Test_All)
