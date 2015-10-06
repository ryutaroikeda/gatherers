#include "dbg.h"
#include "ai.h"
#include "board.h"
#include "cmdline.h"
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

int GTAIMove_ToCommand(GTAIMove* m, const GTBoard* b, GTCommand* c)
{
  int pos = b->units[m->unit].pos;
  c->rank = (pos % GTBoard_WidthMax) - 1;
  c->file = GTBoard_Height - 1 - ((pos / GTBoard_WidthMax) - 1);
  c->cmd = m->cmd;
  c->d = m->dir;
  c->t = m->type;
  return 0;
}

int GTAIMoves_Init(GTAIMoves* m)
{
  m->unitSize = 0;
  memset(m->moveSize, 0, sizeof(int) * GTAI_UnitMax);
  return 0;
}

int GTAI_Init(GTAI* ai) 
{
  ai->p = GTPlayer_None;
  ai->b = NULL;
  ai->err = GTAIError_None;
  return 0;
}

int GTAIMoves_Generate(GTAIMoves* m, const GTBoard* b, int unit)
{
  int i, j;
  int unitSize = m->unitSize;
  GTAIMove* moves = m->moves[unitSize];
  // if the unit has no movement left, nothing to do
  if (!GTBoard_CanStay(b, unit)) { 
    return 0;
  } else {
    GTAIMove* move = &moves[m->moveSize[unitSize]];
    move->unit = unit;
    move->cmd = GTCommandType_Stay;
    m->moveSize[unitSize]++;
  }
  for (i = GTDirection_North; i <= GTDirection_West; i++) {
    if (!GTBoard_CanMoveUnit(b, unit, i)) { continue; }
    GTAIMove* move = &moves[m->moveSize[unitSize]];
    move->unit = unit;
    move->cmd = GTCommandType_Move;
    move->dir = i;
    m->moveSize[unitSize]++;
  }
  if (GTBoard_IsRanged(b, unit)) {
    for (i = GTDirection_North; i < GTDirection_Size; i++) {
      if (!GTBoard_CanRange(b, unit, i)) { continue; }
      GTAIMove* move = &moves[m->moveSize[unitSize]];
      move->unit = unit;
      move->cmd = GTCommandType_Range;
      move->dir = i;
      m->moveSize[unitSize]++;
    }
  }
  if (GTBoard_IsProducer(b, unit)) {
    for (i = GTDirection_North; i <= GTDirection_West; i++) {
      for (j = GTUnitType_Gatherer; j < GTUnitType_Size; j++) {
        if (!GTBoard_CanProduceUnit(b, unit, j, i)) { continue; }
        GTAIMove* move = &moves[m->moveSize[unitSize]];
        move->unit = unit;
        move->cmd = GTCommandType_Produce;
        move->dir = i;
        move->type = j;
        m->moveSize[unitSize]++;
      }
    }
  }
  // m->unitId[unitSize] = unit;
  m->unitSize++;
  return 0;
}

int GTAIMoves_GenerateAll(GTAIMoves* m, const GTBoard* b, GTPlayer p)
{
  int rank, file;
  for (file = GTBoard_Height - 1; file >= 0; file--) {
    for (rank = 0; rank < GTBoard_Width; rank++) {
      int pos = GTBoard_Pos(rank, file);
      if (!GTBoard_IsUnit(b, pos)) { continue; }
      int unit = b->board[pos];
      if (b->units[unit].color != p) { continue; }
      GTAIMoves_Generate(m, b, unit);
    }
  }
  return 0;
}

int GTAI_Random(GTCommand* c)
{
  static GTPlayer p;
  static GTBoard* b;
  static GTAIMoves m;
  if (c->cmd == GTCommandType_SetPlayer) {
    p = (GTPlayer) c->rank;
    return 1;
  } else if (c->cmd == GTCommandType_SetBoard) {
    b = c->b;
    return 2;
  }
  GTAIMoves_Init(&m);
  GTAIMoves_GenerateAll(&m, b, p);
  // no more legal moves, end the turn
  if (m.unitSize == 0) {
    c->cmd = GTCommandType_Done;
    return 0;
  }
  int unit = rand() % m.unitSize;
  int move = rand() % m.moveSize[unit];
  GTAIMove_ToCommand(&m.moves[unit][move], b, c);
  return 0;
}

int GTAI_PlayRandom(GTAI* ai, GTCommand* c)
{
  GTAIMoves m;
  GTAIMoves_Init(&m);
  GTAIMoves_GenerateAll(&m, ai->b, ai->p);
  if (m.unitSize == 0) {
    c->cmd = GTCommandType_Done;
    return 0;
  }
  int unit = rand() % m.unitSize;
  int move = rand() % m.moveSize[unit];
  GTAIMove_ToCommand(&m.moves[unit][move], ai->b, c);
  return 0;
}
