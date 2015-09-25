#include "dbg.h"
#include "gatherers.h"
#include <stddef.h>

// static const GTUnitType tileProduct[GTTileType_Size] =
// {
//   GTUnitType_None,
//   GTUnitType_None,
//   GTUnitType_Archer,
//   GTUnitType_Cavalry,
//   GTUnitType_Spearman,
//   GTUnitType_Castle,
//   GTUnitType_None
// };

static const int unitLife[GTUnitType_Size] =
{
  0, 1, 1, 2, 3, 4
};

// static const int unitCanMove[GTUnitType_Size] =
// {
//   0, 1, 1, 1, 1, 0
// };

// static const int unitMovement[GTUnitType_Size] =
// {
//   0, 1, 1, 2, 1, 1
// };

// static const int unitCanAttack[GTUnitType_Size] =
// {
//   0, 0, 0, 1, 1, 0
// };

// static const int unitRange[GTUnitType_Size] =
// {
//   0, 0, 2, 0, 0, 1
// };

// static const int unitCanProduce[GTUnitType_Size] =
// {
//   0, 1, 0, 0, 0, 1
// };

// static const int direction[GTDirection_Size] =
// {
//   0, -GTBoard_WidthMax, 1, GTBoard_WidthMax, -1
// };

int GTStack_Init(GTStack* s, GTStackEntry* entries, size_t size)
{
  s->size = size;
  s->ptr = 0;
  s->top = entries;
  return 0;
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
  return -1;
}

int GTStack_Peek(GTStack* s, GTStackEntry* e)
{
  check(s->ptr > 0, "nothing to peek");
  e->val = (s->top - 1)->val;
  e->addr = (s->top - 1)->addr;
  return 0;
  error:
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

int GTBoard_Init(GTBoard* b)
{
  memset(b->tiles, 0, sizeof(GTTile) * GTBoard_Size);
  memset(b->board, GTBoard_Empty, sizeof(int) * GTBoard_Size);
  memset(b->entries, 0, sizeof(GTStackEntry) * GTBoard_StackSize);
  GTStack_Init(&b->stack, b->entries, GTBoard_StackSize);
  memset(b->population[0], 0, sizeof(int) * GTPlayer_Size * GTUnitType_Size);
  b->unitId = 0;

  int i;
  for(i = 0; i < GTBoard_ValidMin; i++)
  {
    b->board[i] = GTBoard_Invalid;
  }
  for(i = GTBoard_InvalidMin; i < GTBoard_Size; i++)
  { 
    b->board[i] = GTBoard_Invalid;
  }
  for(i = 0; i < GTBoard_HeightMax; i++)
  {
    b->board[i * GTBoard_WidthMax] = GTBoard_Invalid;
    b->board[(i+1) * GTBoard_WidthMax - 1] = GTBoard_Invalid;
  }
  return 0;
  // error:
  // return -1;
}

int GTBoard_IsValid(GTBoard* b, int pos)
{
  return b->board[pos] != GTBoard_Invalid;
}

int GTBoard_IsEmpty(GTBoard* b, int pos)
{
  return (GTBoard_IsValid(b, pos) && b->board[pos] == GTBoard_Empty);
}

int GTBoard_IsUnit(GTBoard* b, int pos)
{
  return (0 <= b->board[pos] && b->board[pos] < b->unitId
    && GTBoard_IsValid(b, pos));
}

int GTBoard_IsVisible(GTBoard* b, int pos)
{
  return b->tiles[pos].isVisible;
}

int GTBoard_RevealTile(GTBoard* b, int pos)
{
  b->tiles[pos].isVisible = 1;
  return 0;
}

int GTBoard_CreateUnit(GTBoard* b, GTPlayer p, GTUnitType t, int pos)
{
  check(b->unitId < GTBoard_UnitSize, "unit overflow");
  check(GTBoard_IsValid(b, pos), "invalid pos")
  GTUnit* unit = &b->units[b->unitId];
  // GTStack_Push(&(b->stack), unit->color);
  // GTStack_Push(&(b->stack), unit->type);
  // GTStack_Push(&(b->stack), unit->life);
  // GTStack_Push(&(b->stack), unit->movement);
  // GTStack_Push(&(b->stack), b->board[pos]);
  // GTStack_Push(&(b->stack), b->unitId);
  // GTStack_Push(&(b->stack), b->population[p][t]);
  unit->color = p;
  unit->type = t;
  unit->pos = pos;
  unit->life = unitLife[t];
  unit->movement = 0;
  b->board[pos] = b->unitId;
  b->unitId++;
  b->population[p][t]++;
  return 0;
  error:
  return -1;
}

// int GTBoard_RemoveUnit(GTBoard* b, int pos)
// {
//   check(GTBoard_IsUnit(b, pos) == 1, "not a unit");
//   return 0;
// }


