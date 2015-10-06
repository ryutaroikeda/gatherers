#include "dbg.h"
#include "ai.h"
#include "board.h"
#include "cmdline.h"
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

static const int unitScore[GTUnitType_Size] =
{
  0, 0, 4, 3, 2, 1
};

static const int resourceScore[GTTileType_Size] =
{
  0, 0, 3, 4, 6, 8, 1
};

// static const int threatScore[GTAIThreatType_Size] =
// {
//   0, 1, 
// };

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

int GTAI_CopyBoard(GTAI* ai, GTBoard* dst, const GTBoard* src)
{
  (void) ai;
  memcpy(dst->units, src->units, sizeof(int) * GTBoard_UnitSize);
  memcpy(dst->board, src->board, sizeof(int) * GTBoard_Size);
  memcpy(dst->population, src->population,
   sizeof(int) * GTPlayer_Size * GTUnitType_Size);
  memcpy(dst->resources, src->resources,
   sizeof(int) * GTPlayer_Size * GTTileType_Size);
  dst->unitId = src->unitId;
  int i;
  for (i = 0; i < GTBoard_Size; i++) {
    if (!src->tiles[i].isRevealed) { continue; }
    dst->tiles[i] = src->tiles[i];
  }
  return 0;
}

int GTAI_ScoreUnits(GTAI* ai, const GTBoard* b, GTPlayer p)
{
  (void) ai;
  int score = 0;
  int rank, file;
  for (file = GTBoard_Height - 1; file >= 0; file--) {
    for (rank = 0; rank < GTBoard_Width; rank++) {
      int pos = GTBoard_Pos(rank, file);
      if (!GTBoard_IsUnit(b, pos)) { continue; }
      const GTUnit* u = &b->units[b->board[pos]];
      if (u->color != p) { continue; }
      score += unitScore[u->type] * u->life;
    }
  }
  return score;
}

int GTAI_ScoreResources(GTAI* ai, const GTBoard* b, GTPlayer p)
{
  (void) ai;
  int score = 0;
  int i;
  for (i = 0; i < GTTileType_Size; i++) {
    GTUnitType t = tileProduct[i];
    if (t == GTUnitType_None) { continue; }
    int cap = b->resources[p][i] - b->population[p][t];
    if (cap < 0) { cap = 0; }
    score += resourceScore[i] * cap;
  }
  return score;
}

// int GTAI_ScoreThreat(GTAI* ai, const GTBoard* b, const GTUnit* u)
// {
//   int score = 0;
//   if (u->type == GTUnitType_Gatherer) { return score; }
// (void) b;
// (void) u;
// (void)ai;
//   return score;
// }

// int GTAI_ScoreThreats(GTAI* ai, const GTBoard* b, GTPlayer p)
// {
//   (void) ai;
//   int score = 0;
//   int rank, file;
//   for (file = GTBoard_Height - 1; file >= 0; file--) {
//     for (rank = 0; rank < GTBoard_Width; rank++) {
//       int pos = GTBoard_Pos(rank, file);
//       if (!GTBoard_IsUnit(b, pos)) { continue; }
//       const GTUnit* u = &b->units[b->board[pos]];
//       if (u->color != p) { continue; }
//       score += GTAI_ScoreThreat(ai, b, u);
//     }
//   }
//   return score;
// }

int GTAI_DoMove(GTAI* ai, GTBoard* b, const GTAIMove* m)
{
  (void) ai;
  if (m->cmd == GTCommandType_Stay) {
    GTBoard_Stay(b, m->unit);
  } else if (m->cmd == GTCommandType_Move) {
    GTBoard_MoveUnit(b, m->unit, m->dir);
  } else if (m->cmd == GTCommandType_Produce) {
    GTBoard_ProduceUnit(b, m->unit, m->type, m->dir);
  } else if (m->cmd == GTCommandType_Range) {
    GTBoard_Range(b, m->unit, m->dir);
  }
  return 0;
}

int GTAI_ScoreMove(GTAI* ai, GTBoard* b, const GTAIMove* m)
{
  (void) ai;
  int score = 0;
  GTAI_DoMove(ai, b, m);
  score += GTAI_ScoreUnits(ai, b, ai->p);
  score += GTAI_ScoreResources(ai, b, ai->p);
  GTBoard_UndoPlay(b);
  return score;
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

int GTAI_TryScorePlayer(GTCommand* c)
{
  static GTAI ai;
  if (c->cmd == GTCommandType_SetPlayer) {
    log_info("tryscore - setting player");
    ai.p = (GTPlayer) c->rank;
    return 1;
  } else if (c->cmd == GTCommandType_SetBoard) {
    log_info("tryscore - setting board");
    ai.b = c->b;
    return 2;
  }
  log_info("computing move");
  GTAIMoves m;
  GTAIMoves_Init(&m);
  GTAIMoves_GenerateAll(&m, ai.b, ai.p);
  if (m.unitSize == 0) {
    c->cmd = GTCommandType_Done;
    return 0;
  }
  GTBoard brd;
  GTBoard_Init(&brd);
  GTAI_CopyBoard(&ai, &brd, ai.b);
  int i, j;
  int bestScore = 0;
  GTAIMove* best = &m.moves[0][0];
  for (i = 0; i < m.unitSize; i++) {
    for (j = 0; j < m.moveSize[i]; j++) {
      int score = GTAI_ScoreMove(&ai, &brd, &m.moves[i][j]);
      if (score >= bestScore) {
        bestScore = score;
        best = &m.moves[i][j];
      }
    }
  }
  GTAIMove_ToCommand(best, ai.b, c);
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
