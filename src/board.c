#include "dbg.h"
#include "board.h"
#include "lexer.h"
#include <string.h>

static const GTUnitType tileProduct[GTTileType_Size] =
{
  GTUnitType_None,
  GTUnitType_None,
  GTUnitType_Archer,
  GTUnitType_Cavalry,
  GTUnitType_Spearman,
  GTUnitType_Fortress,
  GTUnitType_None
};

static const int unitLife[GTUnitType_Size] =
{
  0, 1, 1, 2, 3, 4
};

static const int unitCanMove[GTUnitType_Size] =
{
  0, 1, 1, 1, 1, 0
};

static const int unitMovement[GTUnitType_Size] =
{
  0, 1, 1, 2, 1, 1
};

static const int unitCanAttack[GTUnitType_Size] =
{
  0, 0, 0, 1, 1, 0
};

static const int unitRange[GTUnitType_Size] =
{
  0, 0, 2, 0, 0, 1
};

static const int unitCanProduce[GTUnitType_Size] =
{
  0, 1, 0, 0, 0, 1
};

const int GTDirection_Direction[GTDirection_Size] =
{
  0,                                    // None
  -GTBoard_WidthMax,                    // North
  1,                                    // East
  GTBoard_WidthMax,                     // South
  -1,                                   // West
  -GTBoard_WidthMax - GTBoard_WidthMax, // NorthNorth
  1 + 1,                                // EastEast
  GTBoard_Width + GTBoard_Width,        // SouthSouth
  -1 - 1,                               // WestWest
  -GTBoard_WidthMax + 1,                // NorthEast
  -GTBoard_WidthMax - 1,                // NorthWest
  GTBoard_WidthMax + 1,                 // SouthEast
  GTBoard_WidthMax - 1                  // SouthWest
};

static const int distance[GTDirection_Size] =
{
  0, 1, 1, 1, 1, 2, 2, 2, 2, 2, 2, 2, 2
};

static const char unitChar[GTPlayer_Size][GTUnitType_Size] =
{
  { ' ', ' ', ' ', ' ', ' ', ' ' },
  { ' ', 'g', 'a', 'c', 's', 'f' },
  { ' ', 'G', 'A', 'C', 'S', 'F' }
};

static const char whitespace[] = " \n\r\t";

static const char emptyUnit[] = "--";

static const char tileChar[2][GTTileType_Size] =
{
  { ' ', 'p', 'w', 'h', 'i', 'l', 'm' },
  { ' ', 'P', 'W', 'H', 'I', 'L', 'M' }
};

// static const char* tokens[] =
// {
//   "",
//   "units",
//   "tiles",
//   "end"
// };

int GTBoard_Init(GTBoard* b)
{
  memset(b->tiles, 0, sizeof(GTTile) * GTBoard_Size);
  memset(b->board, GTBoard_Empty, sizeof(int) * GTBoard_Size);
  memset(b->entries, 0, sizeof(GTStackEntry) * GTBoard_StackSize);
  GTStack_Init(&b->stack, b->entries, GTBoard_StackSize);
  memset(b->population[0], 0, sizeof(int) * GTPlayer_Size * GTUnitType_Size);
  memset(b->resources[0], 0, sizeof(int) * GTPlayer_Size * GTTileType_Size);
  b->unitId = 0;
  b->didProduceProducer = 0;
  b->didProduceUnit = 0;
  b->turn = 0;
  b->err = GTBoardError_None;
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
}

int GTBoard_IsEqual(GTBoard b, GTBoard c)
{
  b.err = c.err;
  b.stack.err = c.stack.err;
  // popping and purging still keep entries on the stack so don't check them
  memcpy(b.entries, c.entries, sizeof(b.entries));
  return memcmp(&b, &c, sizeof(GTBoard)) == 0;
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

int GTBoard_IsRevealed(const GTBoard* b, int pos)
{
  return b->tiles[pos].isRevealed;
}

int GTBoard_IsValidUnit(const GTBoard* b, int unit)
{
  return (0 <= unit && unit < b->unitId &&
   b->board[b->units[unit].pos] == unit);
}

int GTBoard_CanMoveUnit(const GTBoard* b, int unit, GTDirection d)
{
  if (!GTBoard_IsValidUnit(b, unit)) { return 0; }
  if (!unitCanMove[b->units[unit].type]) { return 0; }
  if (b->units[unit].movement <= 0) { return 0; }
  if (distance[d] > 1) { return 0; }
  if (d == GTDirection_None) { return 1; }
  if (!GTBoard_IsValidUnit(b, unit)) { return 0; }
  const GTUnit* u = &b->units[unit];
  int pos = GTDirection_Pos(u->pos, d);
  if (!GTBoard_IsValid(b, pos)) { return 0; }
  if (GTBoard_IsUnit(b, pos)) {
    if (!unitCanAttack[u->type]) { return 0; }
    const GTUnit* v = &b->units[b->board[pos]];
    if (u->color == v->color) { return 0; }
  }
  return 1;
}

int
GTBoard_CanProduceUnit(const GTBoard* b, int unit, GTUnitType t, GTDirection d)
{
  if (t == GTUnitType_None) { return 0; }
  if (!GTBoard_IsValidUnit(b, unit)) { return 0; }
  if (b->didProduceProducer) { return 0; }
  const GTUnit* u = &b->units[unit];
  if (!unitCanProduce[u->type]) { return 0; }
  if (u->movement <= 0) { return 0; }
  int pos = GTDirection_Pos(u->pos, d);
  if (!GTBoard_IsEmpty(b, pos)) { return 0; }
  if (unitCanProduce[t] && b->didProduceUnit) { return 0; }
  GTTileType tile = b->tiles[u->pos].type;
  if (t == GTUnitType_Gatherer) { return 1; }
  if (t != tileProduct[tile]) { return 0; }
  if (b->resources[u->color][tile] <= b->population[u->color][t]) { return 0; }
  return 1;
}

int GTBoard_CanRange(const GTBoard* b, int unit, GTDirection d)
{
  if (!GTBoard_IsValidUnit(b, unit)) { return 0; }
  const GTUnit* u = &b->units[unit];
  if (unitRange[u->type] < distance[d]) { return 0; }
  if (u->movement <= 0) { return 0; }
  int pos = GTDirection_Pos(u->pos, d);
  if (!GTBoard_IsUnit(b, pos)) { return 0; }
  const GTUnit* v = &b->units[b->board[pos]];
  if (u->color == v->color) { return 0; }
  return 1;
}

int GTBoard_RevealTile(GTBoard* b, int pos)
{
  check(GTBoard_IsValid(b, pos), "invalid pos");
  GTStack_PushAndSet(&b->stack, b->tiles[pos].isRevealed, 1);
  return 0;
  error:
  return -1;
}

int GTBoard_CreateUnit(GTBoard* b, GTPlayer p, GTUnitType t, int pos)
{
  check(b->unitId < GTBoard_UnitSize, "unit overflow");
  check(GTBoard_IsValid(b, pos), "invalid pos")
  GTUnit* unit = &b->units[b->unitId];
  GTStack_PushAndSet(&b->stack, unit->color, p);
  GTStack_PushAndSet(&b->stack, unit->type, t);
  GTStack_PushAndSet(&b->stack, unit->pos, pos);
  GTStack_PushAndSet(&b->stack, unit->life, unitLife[t]);
  GTStack_PushAndSet(&b->stack, unit->movement, 0);
  GTStack_PushAndSet(&b->stack, b->board[pos], b->unitId);
  GTStack_PushAndSet(&b->stack, b->unitId, b->unitId + 1);
  GTStack_PushAndSet(&b->stack, b->population[p][t], b->population[p][t] + 1);
  // if unit is a producer, update b->resources
  if (unitCanProduce[t]) {
    GTTileType tile = b->tiles[pos].type;
    GTStack_PushAndSet(&b->stack, b->resources[p][tile],
     b->resources[p][tile] + 1);
  }
  return 0;
  error:
  return -1;
}

int GTBoard_ResetUnitMovement(GTBoard* b, int unit)
{
  check(GTBoard_IsValidUnit(b, unit), "invalid unit");
  GTUnit* u = &b->units[unit];
  GTStack_PushAndSet(&b->stack, u->movement, unitMovement[u->type]);
  return 0;
  error:
  return -1;
}

int GTBoard_DeleteUnit(GTBoard* b, int unit)
{
  check(GTBoard_IsValidUnit(b, unit), "invalid unit");
  GTUnit* u = &b->units[unit];
  if (unitCanProduce[u->type]) {
    GTTileType t = b->tiles[u->pos].type;
    GTStack_PushAndSet(&b->stack, b->resources[u->color][t],
      b->resources[u->color][t] - 1);
  }
  GTStack_PushAndSet(&b->stack, b->population[u->color][u->type],
    b->population[u->color][u->type] - 1);
  GTStack_PushAndSet(&b->stack, b->board[u->pos], GTBoard_Empty);
  GTStack_PushAndSet(&b->stack, u->life, 0);
  GTStack_PushAndSet(&b->stack, u->movement, 0);
  return 0;
  error:
  return -1;
}

int GTBoard_DamageUnit(GTBoard* b, int unit, int damage)
{
  check(GTBoard_IsValidUnit(b, unit), "invalid unit");
  GTUnit* u = &b->units[unit];
  GTStack_PushAndSet(&b->stack, u->life, u->life - damage);
  if (u->life <= 0) {
    GTBoard_DeleteUnit(b, unit);
  }
  return 0;
  error:
  return -1;
}

int GTBoard_MoveUnit(GTBoard* b, int unit, GTDirection d)
{
  check(GTBoard_CanMoveUnit(b, unit, d), "cannot move unit");
  GTStack_BeginPlay(&b->stack);
  GTUnit* u = &b->units[unit];
  int pos = GTDirection_Pos(u->pos, d);
  if (GTBoard_IsUnit(b, pos)) {
    // attack opposing unit at pos
    GTBoard_DamageUnit(b, b->board[pos], 1);
    GTStack_PushAndSet(&b->stack, u->movement, 0);
  }
  if (GTBoard_IsEmpty(b, pos)) {
    // if the unit is a producer, update b->resources
    if (unitCanProduce[u->type])
    {
      GTTileType t = b->tiles[u->pos].type;
      GTStack_PushAndSet(&b->stack, b->resources[u->color][t], 
        b->resources[u->color][t] - 1);
      t = b->tiles[pos].type;
      GTStack_PushAndSet(&b->stack, b->resources[u->color][t],
        b->resources[u->color][t] + 1);
    }
    GTStack_PushAndSet(&b->stack, b->board[pos], unit);
    GTStack_PushAndSet(&b->stack, b->board[u->pos], GTBoard_Empty);
    GTStack_PushAndSet(&b->stack, u->pos, pos);
    GTStack_PushAndSet(&b->stack, u->movement, u->movement - 1);
    GTBoard_RevealTile(b, pos);
  }
  if (b->tiles[pos].type == GTTileType_Mountain) {
    GTStack_PushAndSet(&b->stack, u->movement, u->movement - 1);
  }
  return 0;
  error:
  b->err = GTBoardError_CannotMoveUnit;
  return -1;
}

int GTBoard_ProduceUnit(GTBoard* b, int unit, GTUnitType t, GTDirection d)
{
  check(GTBoard_IsValidUnit(b, unit), "invalid unit");
  check(GTBoard_CanProduceUnit(b, unit, t, d), "unit cannot produce");
  GTUnit* u = &b->units[unit];
  int pos = GTDirection_Pos(u->pos, d);
  GTStack_BeginPlay(&b->stack);
  GTBoard_CreateUnit(b, u->color, t, pos);
  GTStack_PushAndSet(&b->stack, u->movement, 0);
  GTStack_PushAndSet(&b->stack, b->didProduceUnit, 1);
  if (unitCanProduce[t]) {
    GTStack_PushAndSet(&b->stack, b->didProduceProducer, 1);
  }
  GTBoard_RevealTile(b, pos);
  return 0;
  error:
  return -1;
}

int GTBoard_Range(GTBoard* b, int unit, GTDirection d)
{
  check(GTBoard_CanRange(b, unit, d), "can't range");
  GTStack_BeginPlay(&b->stack);
  GTUnit* u = &b->units[unit];
  GTStack_PushAndSet(&b->stack, u->movement, 0);
  // check if we are ranging over a mountain
  if ((GTDirection_NorthNorth <= d) && (d <= GTDirection_WestWest)) {
    // convert NorthNorth to North, etc.
    GTDirection e = d - 4;
    int p = GTDirection_Pos(u->pos, e);
    GTBoard_RevealTile(b, p);
    check_debug(b->tiles[p].type != GTTileType_Mountain, "found mountain");
  }
  int pos = GTDirection_Pos(u->pos, d);
  int target = b->board[pos];
  GTBoard_DamageUnit(b, target, 1);
  return 0;
  error:
  return -1;
}

int GTBoard_UndoPlay(GTBoard* b)
{
  GTStackEntry e;
  b->stack.err = GTStackError_None;
  check(!GTStack_IsEmpty(&b->stack), "empty stack");
  do {
    GTStack_Peek(&b->stack, &e);
    if (e.addr == NULL) {
      check(e.val == GTStackCode_BeginPlay, "bad code");
      GTStack_Purge(&b->stack);
      break;
    }
    GTStack_Pop(&b->stack);
  } while (!((e.val == GTStackCode_BeginPlay) && (e.addr == NULL))
    && b->stack.err == GTStackError_None);
  check(b->stack.err == GTStackError_None, "stack error");
  return 0;
  error:
  b->err = GTBoardError_FailedUndoPlay;
  return -1;
}

int GTBoard_EndTurn(GTBoard* b)
{
  GTStack_PushAndSet(&b->stack, b->turn, b->turn + 1);
  int i;
  for (i = GTBoard_ValidMin; i < GTBoard_InvalidMin; i++) {
    int unit = b->board[i];
    if (!GTBoard_IsValidUnit(b, unit)) { continue; }
    GTBoard_ResetUnitMovement(b, unit);
  }
  GTStack_BeginTurn(&b->stack);
  check(b->stack.err == GTStackError_None, "stack error");
  return 0;
  error:
  return -1;
}

int GTBoard_ParseUnit(GTBoard* b, char* tok, int pos)
{
  char u = tok[0];
  char l = tok[1];
  GTPlayer p = GTPlayer_None;
  if ('a' <= u && u <= 'z') {
    p = GTPlayer_Black;
  } else if ('A' <= u && u <= 'Z') {
    p = GTPlayer_White;
  } else {
    sentinel("invalid unit token %c", u);
  }
  GTUnitType t = GTUnitType_None;
  int i;
  for (i = 0; i < GTUnitType_Size; i++) {
    if (u != unitChar[p][i]) { continue; }
    t = i;
  }
  check(t != GTUnitType_None, "unrecognized unit %c", u);
  int life = l - '0';
  check(life > 0, "life = %d too low", life);
  check(GTBoard_CreateUnit(b, p, t, pos) == 0, "create unit failed");
  b->units[b->board[pos]].life = life;
  return 0;
  error:
  return -1;
}

int GTBoard_ParseUnits(GTBoard* b, char* s)
{
  int file, rank;
  GTLexer l;
  GTLexer_Init(&l, s);
  for (file = GTBoard_Height - 1; file >= 0; file--) {
    for(rank = 0; rank < GTBoard_Width; rank++) {
      GTLexer_Skip(&l, whitespace);
      char* tok = GTLexer_GetToken(&l, ",");
      check(strlen(tok) == 2, "unexpected token %.*s", 80, tok);
      if (strcmp(tok, emptyUnit) == 0) { continue; }
      int pos = GTBoard_Pos(rank, file);
      check(GTBoard_ParseUnit(b, tok, pos) == 0, "corrupt unit");
    }
  }
  return 0;
  error:
  return -1;
}

int GTBoard_ParseTile(GTBoard* b, char tok, int pos)
{
  int isRevealed;
  int i;
  if ('a' <= tok && tok <= 'z') {
    isRevealed = 0;
  } else {
    isRevealed = 1;
  }
  for (i = 0; i < GTTileType_Size; i++) {
    if (tok != tileChar[isRevealed][i]) { continue; }
    b->tiles[pos].type = i;
    b->tiles[pos].isRevealed = isRevealed;
    return 0;
  }
  return -1;
}

int GTBoard_ParseTiles(GTBoard* b, char* s)
{
  int file, rank;
  GTLexer l;
  GTLexer_Init(&l, s);
  for (file = GTBoard_Height -1; file >= 0; file--) {
    for (rank = 0; rank < GTBoard_Width; rank++) {
      GTLexer_Skip(&l, whitespace);
      char* tok = GTLexer_GetToken(&l, ",");
      check(strlen(tok) == 1, "unexpected token %.*s", 80, tok);
      int pos = GTBoard_Pos(rank, file);
      check(GTBoard_ParseTile(b, tok[0], pos) == 0, "corrupt tile");
    }
  }
  return 0;
  error:
  return -1;
}

// int GTBoard_Parse(GTBoard* b, char* s)
// {
//   int i, j;

// }

// int GTBoard_Get(GTBoard* b, GTCharGetter cg)
// {
//   return 0;
// }

int GTBoard_Print(const GTBoard* b)
{
  fprintf(stdout, "\n  a  b  c  d  e\n");
  int rank, file;
  for (file = GTBoard_Height - 1; file >= 0; file--) {
    fprintf(stdout, "%d", file + 1);
    for (rank = 0; rank < GTBoard_Width; rank++) {
      int pos = GTBoard_Pos(rank, file);
      if (GTBoard_IsEmpty(b, pos)) {
        fprintf(stdout, " . ");
      } else {
        const GTUnit* u = &b->units[b->board[pos]];
        fprintf(stdout, " %c%d", unitChar[u->color][u->type], u->life);
      }
    }
    fprintf(stdout, "\n");
  }
  fprintf(stdout, "\n");
  return 0;
}

