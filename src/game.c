#include "dbg.h"
#include "game.h"

int GTGame_Init(GTGame* g)
{
  g->p = GTPlayer_Black;
  GTBoard_Init(&g->b);
  GTBoard_CreateUnit(&g->b, GTPlayer_Black, GTUnitType_Gatherer, 
    GTGame_BlackStart);
  GTBoard_CreateUnit(&g->b, GTPlayer_White, GTUnitType_Gatherer,
    GTGame_WhiteStart);
  GTStack_BeginTurn(&g->b.stack);
  check(g->b.err == GTBoardError_None, "board error");
  return 0;
  error:
  return -1;
}

int GTGame_IsEnd(const GTGame* g)
{
  return ((g->b.population[GTPlayer_Black][GTUnitType_Gatherer] <= 0)
    || (g->b.population[GTPlayer_White][GTUnitType_Gatherer] <= 0));
}

int GTGame_DoCommand(GTGame* g, GTCommand* c)
{
  c->err = GTCommandError_None;
  if (c->cmd == GTCommandType_Exit) {
    c->err = GTCommandError_Exit;
    return -1;
  }
  check(c->cmd != GTCommandType_None, "command needed");
  if (c->cmd == GTCommandType_Done) { 
    GTGame_EndTurn(g);
    return 0;
  }
  int pos = GTBoard_Pos(c->rank, c->file);
  int unit = g->b.board[pos];
  GTPlayer p = g->b.units[unit].color;
  check(g->p == p, "tried moving wrong color");
  int err = 0;
  if (c->cmd == GTCommandType_Move) {
    err = GTBoard_MoveUnit(&g->b, unit, c->d);
  } else if (c->cmd == GTCommandType_Range) {
    err = GTBoard_Range(&g->b, unit, c->d);
  } else if (c->cmd == GTCommandType_Produce) {
    err = GTBoard_ProduceUnit(&g->b, unit, c->t, c->d);
  }
  check(err == 0, "error with command");
  return 0;
  error:
  c->err = GTCommandError_Illegal;
  return -1;
}

int GTGame_EndTurn(GTGame* g)
{
  check(GTBoard_EndTurn(&g->b) == 0, "GTBoard_EndTurn failed");
  if (g->p == GTPlayer_Black) {
    g->p = GTPlayer_White;
  } else {
    g->p = GTPlayer_Black;
  }
  return 0;
  error:
  return -1;
}

// int GTGame_Play(GTGame* g)
// {
//   return 0;
// }

