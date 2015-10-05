#include "minunit.h"
#include "netcmd.h"

static char* Test_Parse()
{
  char s[] = "game: info";
  GTNetCommand nc;
  GTNetCommand_Init(&nc);
  mu_assert(GTNetCommand_Parse(&nc, s) == 0, "parse failed");
  mu_assert(nc.type == GTNetCommandType_Game, ".type wrong");
  mu_assert(nc.game.cmd == GTCommandType_Info, ".game.cmd wrong");
  return NULL;
}

static char* Test_All()
{
  mu_suite_start();
  mu_run_test(Test_Parse);
  return NULL;
}

RUN_TESTS(Test_All)
