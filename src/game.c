#include "dbg.h"
#include "game.h"
#include "cmdline.h"
#include "board.h"
#include "writer.h"

int GTGame_Init(GTGame* g, GTBoard* b)
{
  g->b = b;
  g->p = GTPlayer_Black;
  memset(g->interface, 0, sizeof(GTCommandGetter) * GTPlayer_Size);
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
  check_debug(c->cmd != GTCommandType_None, "command needed");
  if (c->cmd == GTCommandType_Exit) {
    c->err = GTCommandError_Exit;
    return -1;
  } else if (c->cmd == GTCommandType_Done) { 
    GTGame_EndTurn(g);
    return 0;
  } else if (c->cmd == GTCommandType_Undo) {
    check_debug(GTBoard_UndoPlay(g->b) == 0, "cannot undo");
    return 0;
  } else if (c->cmd == GTCommandType_Info) {
    GTGame_PrintInfo(g, stdout);
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
    log_info("moving unit %d", unit);
    err = GTBoard_MoveUnit(g->b, unit, c->d);
  } else if (c->cmd == GTCommandType_Range) {
    log_info("ranging with %d", unit);
    err = GTBoard_Range(g->b, unit, c->d);
  } else if (c->cmd == GTCommandType_Produce) {
    log_info("producing with %d", unit);
    err = GTBoard_ProduceUnit(g->b, unit, c->t, c->d);
  } else if (c->cmd == GTCommandType_Stay) {
    log_info("unit %d stays", unit);
    err = GTBoard_Stay(g->b, unit);
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

int GTGame_GetCommand(GTGame* g, GTCommand* c)
{
  check((*g->interface[g->p])(c) == 0, "error getting command");
  return 0;
  error:
  return -1;
}

int GTGame_Play(GTGame* g)
{
  // end turn zero to reset unit movement
  GTBoard_EndTurn(g->b);
  while (1) {
    GTCommand c;
    GTCommand_Init(&c);
    GTGame_Print(g, stdout);
    fprintf(stdout, "player %d: ", g->p);
    if (GTGame_GetCommand(g, &c) == -1) {
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

int GTGame_PlayStdin(GTGame* g)
{
  g->interface[GTPlayer_Black] = &GTCommand_GetStdin;
  g->interface[GTPlayer_White] = &GTCommand_GetStdin;
  return GTGame_Play(g);
}

int GTGame_Print(const GTGame* g, FILE* stream)
{
  GTWriter w;
  GTWriter_InitFile(&w, stream);
  GTBoard_Print(g->b, &w);
  return 0;
}

int GTGame_PrintInfo(const GTGame* g, FILE* stream) 
{
  GTWriter w;
  GTWriter_InitFile(&w, stream);
  GTBoard_PrintDemographics(g->b, &w);
  GTBoard_PrintTiles(g->b, &w);
  return 0;
}
