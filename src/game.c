#include "dbg.h"
#include "game.h"
#include "cmdline.h"
#include "board.h"

int GTGame_Init(GTGame* g, GTBoard* b)
{
  g->b = b;
  g->p = GTPlayer_Black;
  check(g->b->err == GTBoardError_None, "board error");
  return 0;
  error:
  return -1;
}

int GTGame_IsEnd(const GTGame* g)
{
  return ((g->b->population[GTPlayer_Black][GTUnitType_Gatherer] <= 0)
    || (g->b->population[GTPlayer_White][GTUnitType_Gatherer] <= 0));
}

//
// Note: GTCommand* c should not be trusted, especially .file and .rank that
// are used for accessing arrays.
//

int GTGame_DoCommand(GTGame* g, GTCommand* c)
{
  c->err = GTCommandError_None;
  if (c->cmd == GTCommandType_Exit) {
    c->err = GTCommandError_Exit;
    return -1;
  }
  check_debug(c->cmd != GTCommandType_None, "command needed");
  if (c->cmd == GTCommandType_Done) { 
    GTGame_EndTurn(g);
    return 0;
  }
  int pos = GTBoard_Pos(c->rank, c->file);
  check_debug(GTBoard_IsValid(g->b, pos), "invalid pos");
  int unit = g->b->board[pos];
  check_debug(GTBoard_IsValidUnit(g->b, unit), "invalid unit");
  GTPlayer p = g->b->units[unit].color;
  check_debug(g->p == p, "tried moving wrong color");
  int err = 0;
  if (c->cmd == GTCommandType_Move) {
    err = GTBoard_MoveUnit(g->b, unit, c->d);
  } else if (c->cmd == GTCommandType_Range) {
    err = GTBoard_Range(g->b, unit, c->d);
  } else if (c->cmd == GTCommandType_Produce) {
    err = GTBoard_ProduceUnit(g->b, unit, c->t, c->d);
  }
  check_debug(err == 0, "error with command");
  return 0;
  error:
  c->err = GTCommandError_Illegal;
  return -1;
}

int GTGame_EndTurn(GTGame* g)
{
  check(GTBoard_EndTurn(g->b) == 0, "GTBoard_EndTurn failed");
  if (g->p == GTPlayer_Black) {
    g->p = GTPlayer_White;
  } else {
    g->p = GTPlayer_Black;
  }
  return 0;
  error:
  return -1;
}

int GTGame_Play(GTGame* g)
{
  while (1) {
    GTCommand c;
    if (GTCommand_GetStdin(&c) == -1) {
      fprintf(stdout, "bad command, try again\n");
      continue;
    }
    GTGame_DoCommand(g, &c);
    if (c.err == GTCommandError_Illegal) {
      fprintf(stdout, "Illegal move, try again\n");
    } else if (c.err == GTCommandError_Exit) {
      fprintf(stdout, "player %d resigns\n", g->p);
      break;
    }
    if (GTGame_IsEnd(g)) {
      fprintf(stdout, "player %d wins\n", g->p);
      break;
    }
  }
  return 0;
}

