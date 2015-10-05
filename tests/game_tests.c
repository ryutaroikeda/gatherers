#include "minunit.h"
#include "game.h"
#include "board.h"
#include "cmdline.h"
#include "writer.h"

static char* Test_GTGame_Init()
{
  GTGame g;
  GTBoard b;
  GTGame_Init(&g, &b);
  mu_assert(g.p = GTPlayer_Black, ".p(layer) wrong");
  return NULL;
}

static char* Test_GTGame_IsEnd()
{
  GTGame g;
  GTBoard b;
  GTBoard_Init(&b);
  GTGame_Init(&g, &b);
  mu_assert(GTGame_IsEnd(&g), "game not ended");
  GTBoard_CreateUnit(&b, GTPlayer_Black, GTUnitType_Gatherer, GTBoard_ValidMin);
  mu_assert(GTGame_IsEnd(&g), "game not ended");
  GTBoard_CreateUnit(&b, GTPlayer_White, GTUnitType_Gatherer,
   GTBoard_ValidMin + 1);
  mu_assert(!GTGame_IsEnd(&g), "game is ended");
  return NULL;
}

static char* Test_GTGame_DoCommand()
{
  GTGame g;
  GTBoard b;
  GTBoard_Init(&b);
  GTGame_Init(&g, &b);
  GTCommand c;
  GTCommand_Init(&c);
  GTWriter w;
  GTWriter_InitFile(&w, stdout);
  mu_assert(GTGame_DoCommand(&g, &c, &w) == -1, "illegal command valid");
  c.cmd = GTCommandType_Exit;
  mu_assert(GTGame_DoCommand(&g, &c, &w) == -1, "exit didn't return -1");
  mu_assert(c.err == GTCommandError_Exit, ".err wrong");
  c.cmd = GTCommandType_Done;
  mu_assert(GTGame_DoCommand(&g, &c, &w) == 0, "done failed");
  return NULL;
}

static char* Test_GTGame_EndTurn()
{
  GTGame g;
  GTBoard b;
  GTBoard_Init(&b);
  GTGame_Init(&g, &b);
  mu_assert(GTGame_EndTurn(&g) == 0, "endturn failed");
  mu_assert(g.p == GTPlayer_White, ".p wrong");
  mu_assert(GTGame_EndTurn(&g) == 0, "end turn failed");
  mu_assert(g.p == GTPlayer_Black, ".p wrong");
  return NULL;
}

static char* Test_All()
{
  mu_suite_start();
  mu_run_test(Test_GTGame_Init);
  mu_run_test(Test_GTGame_IsEnd);
  mu_run_test(Test_GTGame_DoCommand);
  mu_run_test(Test_GTGame_EndTurn);
  return NULL;
}

RUN_TESTS(Test_All)
