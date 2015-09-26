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

static const int unitMovement[GTUnitType_Size] =
{
  0, 1, 1, 2, 1, 1
};

static const int unitCanAttack[GTUnitType_Size] =
{
  0, 0, 0, 1, 1, 0
};

// static const int unitRange[GTUnitType_Size] =
// {
//   0, 0, 2, 0, 0, 1
// };

// static const int unitCanProduce[GTUnitType_Size] =
// {
//   0, 1, 0, 0, 0, 1
// };

const int GTDirection_Direction[GTDirection_Size] =
{
  0, -GTBoard_WidthMax, 1, GTBoard_WidthMax, -1
};

int GTPlayer_IsOpposite(GTPlayer p, GTPlayer q)
{
  return ((p == GTPlayer_Black && q == GTPlayer_White) ||
    (p == GTPlayer_White && q == GTPlayer_Black));
}

int GTStack_Init(GTStack* s, GTStackEntry* entries, size_t size)
{
  s->size = size;
  s->ptr = 0;
  s->err = GTStackError_None;
  s->top = entries;
  return 0;
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

int GTBoard_Init(GTBoard* b)
{
  memset(b->tiles, 0, sizeof(GTTile) * GTBoard_Size);
  memset(b->board, GTBoard_Empty, sizeof(int) * GTBoard_Size);
  memset(b->entries, 0, sizeof(GTStackEntry) * GTBoard_StackSize);
  GTStack_Init(&b->stack, b->entries, GTBoard_StackSize);
  memset(b->population[0], 0, sizeof(int) * GTPlayer_Size * GTUnitType_Size);
  b->unitId = 0;
  int i;
  for(i = 0; i < GTBoard_ValidMin; i++) {
    b->board[i] = GTBoard_Invalid;
  }
  for(i = GTBoard_InvalidMin; i < GTBoard_Size; i++) { 
    b->board[i] = GTBoard_Invalid;
  }
  for(i = 0; i < GTBoard_HeightMax; i++) {
    b->board[i * GTBoard_WidthMax] = GTBoard_Invalid;
    b->board[(i+1) * GTBoard_WidthMax - 1] = GTBoard_Invalid;
  }
  return 0;
  // error:
  // return -1;
}

int GTBoard_IsValid(const GTBoard* b, int pos)
{
  return b->board[pos] != GTBoard_Invalid;
}

int GTBoard_IsEmpty(const GTBoard* b, int pos)
{
  return (GTBoard_IsValid(b, pos) && b->board[pos] == GTBoard_Empty);
}

int GTBoard_IsUnit(const GTBoard* b, int pos)
{
  return (0 <= b->board[pos] && b->board[pos] < b->unitId
    && GTBoard_IsValid(b, pos));
}

int GTBoard_IsVisible(const GTBoard* b, int pos)
{
  return b->tiles[pos].isVisible;
}

int GTBoard_IsValidUnit(const GTBoard* b, int unit)
{
  return (0 <= unit && unit < b->unitId &&
   b->board[b->units[unit].pos] == unit);
}

int GTBoard_CanMoveUnit(const GTBoard* b, int unit, GTDirection d)
{
  if (!GTBoard_IsValidUnit(b, unit)) { return 0; }
  if (b->units[unit].movement <= 0) { return 0; }
  if (d == GTDirection_None) { return 1; }
  if (!GTBoard_IsValidUnit(b, unit)) { return 0; }
  const GTUnit* u = &(b->units[unit]);
  int pos = GTDirection_Pos(u->pos, d);
  if (!GTBoard_IsValid(b, pos)) { return 0; }
  if (!GTBoard_IsEmpty(b, pos)) {
    if (!unitCanAttack[u->type]) { return 0; }
    const GTUnit* v = &(b->units[b->board[pos]]);
    if (u->color == v->color) { return 0; }
  }
  return 1;
}

int GTBoard_RevealTile(GTBoard* b, int pos)
{
  check(GTBoard_IsValid(b, pos), "invalid pos");
  GTStack_Push(&(b->stack), b->tiles[pos].isVisible);
  b->tiles[pos].isVisible = 1;
  return 0;
  error:
  return -1;
}

int GTBoard_CreateUnit(GTBoard* b, GTPlayer p, GTUnitType t, int pos)
{
  check(b->unitId < GTBoard_UnitSize, "unit overflow");
  check(GTBoard_IsValid(b, pos), "invalid pos")
  GTUnit* unit = &b->units[b->unitId];
  GTStack_Push(&(b->stack), unit->color);
  GTStack_Push(&(b->stack), unit->type);
  GTStack_Push(&(b->stack), unit->life);
  GTStack_Push(&(b->stack), unit->pos);
  GTStack_Push(&(b->stack), unit->movement);
  GTStack_Push(&(b->stack), b->board[pos]);
  GTStack_Push(&(b->stack), b->unitId);
  GTStack_Push(&(b->stack), b->population[p][t]);
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

int GTBoard_ResetUnitMovement(GTBoard* b, int unit)
{
  check(GTBoard_IsValidUnit(b, unit), "invalid unit");
  GTUnit* u = &b->units[unit];
  GTStack_Push(&(b->stack), u->movement);
  u->movement = unitMovement[u->type];
  return 0;
  error:
  return -1;
}

int GTBoard_RemoveUnit(GTBoard* b, int unit)
{
  check(GTBoard_IsValidUnit(b, unit), "invalid unit");
  GTUnit* u = &b->units[unit];
  GTStack_Push(&(b->stack), b->board[u->pos]);
  GTStack_Push(&(b->stack), u->life);
  GTStack_Push(&(b->stack), u->movement);
  b->board[u->pos] = GTBoard_Empty;
  u->life = 0;
  u->movement = 0;
  return 0;
  error:
  return -1;
}

int GTBoard_DamageUnit(GTBoard* b, int unit, int damage)
{
  check(GTBoard_IsValidUnit(b, unit), "invalid unit");
  GTUnit* u = &b->units[unit];
  GTStack_Push(&(b->stack), u->life);
  u->life -= damage;
  if(u->life <= 0) {
    GTBoard_RemoveUnit(b, unit);
  }
  return 0;
  error:
  return -1;
}

int GTBoard_MoveUnit(GTBoard* b, int unit, GTDirection d)
{
  check(GTBoard_CanMoveUnit(b, unit, d), "cannot move unit");
  GTStack_BeginPlay(&(b->stack));
  GTUnit* u = &b->units[unit];
  int pos = GTDirection_Pos(u->pos, d);
  if (!GTBoard_IsEmpty(b, pos)) {
    // attack opposing unit at pos
    GTBoard_DamageUnit(b, b->board[pos], 1);
    GTStack_Push(&(b->stack), u->movement);
    u->movement--;
  }
  if (GTBoard_IsEmpty(b, pos)) {
    GTStack_Push(&(b->stack), b->board[pos]);
    GTStack_Push(&(b->stack), b->board[u->pos]);
    GTStack_Push(&(b->stack), u->pos);
    GTStack_Push(&(b->stack), u->movement);
    b->board[pos] = unit;
    b->board[u->pos] = GTBoard_Empty;
    u->pos = pos;
    u->movement--;
    GTBoard_RevealTile(b, pos);
  }
  if (b->tiles[pos].type == GTTileType_Mountain) {
    GTStack_Push(&(b->stack), u->movement);
    u->movement--; 
  }
  return 0;
  error:
  b->err = GTBoardError_CannotMoveUnit;
  return -1;
}

int GTBoard_UndoPlay(GTBoard* b)
{
  GTStackEntry e;
  b->stack.err = GTStackError_None;
  check(!GTStack_IsEmpty(&(b->stack)), "empty stack");
  do {
    GTStack_Peek(&(b->stack), &e);
    if (e.addr == NULL) {
      check(e.val == GTStackCode_BeginPlay, "bad code");
      GTStack_Purge(&(b->stack));
      break;
    }
    GTStack_Pop(&(b->stack));
  } while (!((e.val == GTStackCode_BeginPlay) && (e.addr == NULL))
    && b->stack.err == GTStackError_None);
  check(b->stack.err == GTStackError_None, "stack error");
  return 0;
  error:
  b->err = GTBoardError_FailedUndoPlay;
  return -1;
}

