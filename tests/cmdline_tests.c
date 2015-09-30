#include "minunit.h"
#include "cmdline.h"

static char* Test_GTCommand_Init()
{
  GTCommand c;
  GTCommand_Init(&c);
  mu_assert(c.cmd == GTCommandType_None, ".c wrong");
  return NULL;
}

static char* Test_GTCommand_ParsePos()
{
  GTCommand c;
  char* tok = "b4";
  mu_assert(GTCommand_ParsePos(&c, tok) == 0, "parse failed");
  mu_assert(c.rank == 1, ".rank wrong");
  mu_assert(c.file == 3, ".file wrong");
  tok = "9834";
  mu_assert(GTCommand_ParsePos(&c, tok) == -1, "parsed bad string");
  return NULL;
}

static char* Test_GTCommand_ParseCmd()
{
  GTCommand c;
  char* tok = "mv";
  mu_assert(GTCommand_ParseCmd(&c, tok) == 0, "parse failed");
  mu_assert(c.cmd == GTCommandType_Move, ".cmd wrong");
  tok = "mv4";
  mu_assert(GTCommand_ParseCmd(&c, tok) == -1, "parsed bad string");
  tok = "m v";
  mu_assert(GTCommand_ParseCmd(&c, tok) == -1, "parsed bad string");
  return NULL;
}

static char* Test_GTCommand_ParseUnit()
{
  GTCommand c;
  char* tok = "f";
  mu_assert(GTCommand_ParseUnit(&c, tok) == 0, "parse failed");
  mu_assert(c.t == GTUnitType_Fortress, ".t wrong");
  tok = "g5";
  mu_assert(GTCommand_ParseUnit(&c, tok) == -1, "parsed bad string");
  return NULL;
}

static char* Test_GTCommand_ParseDir()
{
  GTCommand c;
  char* tok = "nn";
  mu_assert(GTCommand_ParseDir(&c, tok) == 0, "parse failed");
  mu_assert(c.d == GTDirection_NorthNorth, ".d wrong");
  tok = "sse";
  mu_assert(GTCommand_ParseUnit(&c, tok) == -1, "parsed bad string");
  return NULL;
}

static char* Test_GTCommand_Parse()
{
  GTCommand c;
  char s[] = "rg d3 s";
  mu_assert(GTCommand_Parse(&c, s) == 0, "parse failed");
  mu_assert(c.rank == 3, ".rank wrong");
  mu_assert(c.file == 2, ".file wrong");
  mu_assert(c.cmd == GTCommandType_Range, ".cmd wrong");
  mu_assert(c.d == GTDirection_South, ".d wrong");
  char t[] = "  \n \r    exit";
  mu_assert(GTCommand_Parse(&c, t) == 0, "parse failed");
  mu_assert(c.cmd == GTCommandType_Exit, ".cmd wrong");
  char u[] = " \r  pd\r a1 \n ee \n s";
  mu_assert(GTCommand_Parse(&c, u) == 0, "parse failed");
  mu_assert(c.rank == 0, ".rank wrong");
  mu_assert(c.file == 0, ".file wrong");
  mu_assert(c.cmd == GTCommandType_Produce, ".cmd wrong");
  mu_assert(c.d == GTDirection_EastEast, ".d wrong");
  mu_assert(c.t == GTUnitType_Spearman, ".t wrong");
  char v[] = "e2 mv n";
  mu_assert(GTCommand_Parse(&c, v) == -1, "parsed bad string");
  return NULL;
}

static char* Test_All()
{
  mu_suite_start();
  mu_run_test(Test_GTCommand_Init);
  mu_run_test(Test_GTCommand_ParsePos);
  mu_run_test(Test_GTCommand_ParseCmd);
  mu_run_test(Test_GTCommand_ParseUnit);
  mu_run_test(Test_GTCommand_ParseDir);
  mu_run_test(Test_GTCommand_Parse);
  return NULL;
}

RUN_TESTS(Test_All)
