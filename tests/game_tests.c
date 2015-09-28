#include "minunit.h"
#include "game.h"

static char* Test_GTGame_Init()
{
  GTGame g;
  GTGame_Init(&g);
  mu_assert(g.p = GTPlayer_Black, ".p(layer) wrong");
  mu_assert(g.b.board[GTGame_BlackStart] == 0, ".b.board wrong");
  mu_assert(g.b.board[GTGame_WhiteStart] == 1, ".b.board wrong");
  return NULL;
}

static char* Test_GTGame_IsEnd()
{
  GTGame g;
  GTGame_Init(&g);
  mu_assert(!GTGame_IsEnd(&g), "game is ended");
  return NULL;
}

static char* Test_GTGame_DoCommand()
{
  GTGame g;
  GTGame_Init(&g);
  GTCommand c;
  GTCommand_Init(&c);
  mu_assert(GTGame_DoCommand(&g, &c) == -1, "illegal cmdmand valid");
  c.cmd = GTCommandType_Exit;
  mu_assert(GTGame_DoCommand(&g, &c) == -1, "exit didn't return -1");
  mu_assert(c.err == GTCommandError_Exit, ".err wrong");
  c.cmd = GTCommandType_Done;
  mu_assert(GTGame_DoCommand(&g, &c) == 0, "done failed");
  return NULL;
}

static char* Test_GTGame_EndTurn()
{
  GTGame g;
  GTGame_Init(&g);
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
