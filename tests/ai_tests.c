#include "minunit.h"
#include "ai.h"
#include "board.h"

static char* Test_GTAIMove_ToCommand()
{
  GTBoard b;
  GTBoard_Init(&b);
  GTBoard_CreateUnit(&b, GTPlayer_None, GTUnitType_Gatherer, GTBoard_ValidMin);
  GTAIMove m;
  m.unit = 0;
  GTCommand c;
  GTCommand_Init(&c);
  mu_assert(GTAIMove_ToCommand(&m, &b, &c) == 0, "to command failed");
  mu_assert(c.rank == 0, ".rank wrong");
  mu_assert(c.file == GTBoard_Height - 1, ".file wrong");
  return NULL;
}

static char* Test_GTAIMoves_Generate()
{
  GTBoard b;
  GTAIMoves m;
  char file[] =
  "tiles {"
  "p,p,p,p,p,"
  "p,p,p,p,p,"
  "p,p,p,p,p,"
  "p,p,p,p,p,"
  "p,p,p,p,p,"
  "p,p,p,p,p,}"
  "units {"
  "--,--,--,--,--,"
  "--,--,--,--,--,"
  "--,--,--,--,--,"
  "--,--,g1,--,--,"
  "--,--,--,--,--,"
  "--,--,--,--,--,}";
  GTBoard_Init(&b);
  GTAIMoves_Init(&m);
  GTBoard_CreateUnit(&b, GTPlayer_None, GTUnitType_Gatherer, GTBoard_ValidMin);
  mu_assert(GTAIMoves_Generate(&m, &b, 0) == 0, "generate failed");
  mu_assert(m.unitSize == 0, ".unitSize wrong");
  GTBoard_Init(&b);
  GTAIMoves_Init(&m);
  mu_assert(GTBoard_Parse(&b, file) == 0, "parse error");
  mu_assert(GTAIMoves_Generate(&m, &b, 0) == 0, "generate failed");
  mu_assert(m.unitSize == 0, ".unitSize wrong");
  mu_assert(m.moveSize[0] == 0, ".moveSize wrong");
  GTBoard_ResetUnitMovement(&b, 0);
  mu_assert(GTAIMoves_Generate(&m, &b, 0) == 0, "generate failed");
  mu_assert(m.unitSize == 1, ".unitSize wrong");
  mu_assert(m.moveSize[0] == 9, ".moveSize wrong");
  return NULL;
}

static char* Test_GTAIMoves_GenerateAll()
{
  GTBoard b;
  GTAIMoves m;
  char file[] =
  "tiles {"
  "p,p,p,p,p,"
  "p,p,p,p,p,"
  "p,p,p,p,p,"
  "p,p,p,p,p,"
  "p,p,p,p,p,"
  "p,p,p,p,p,}"
  "units {"
  "--,--,--,--,--,"
  "--,c2,G1,--,--,"
  "--,--,--,--,--,"
  "--,--,a1,--,--,"
  "--,--,--,--,--,"
  "--,--,--,--,--,}";
  GTBoard_Init(&b);
  GTAIMoves_Init(&m);
  GTAIMoves_GenerateAll(&m, &b, GTPlayer_Black);
  mu_assert(m.unitSize == 0, ".unitSize wrong");
  mu_assert(GTBoard_Parse(&b, file) == 0, "parse error");
  GTBoard_ResetUnitMovement(&b, 0);
  GTBoard_ResetUnitMovement(&b, 2);
  mu_assert(GTAIMoves_GenerateAll(&m, &b, GTPlayer_Black) == 0,
   "generate failed");
  mu_assert(m.unitSize == 2, ".unitSize wrong");
  mu_assert(m.moveSize[0] == 5, ".moveSize for cavalry wrong");
  mu_assert(m.moveSize[1] == 6, ".moveSize for archer wrong");
  return NULL;
}

static char* Test_GTAIThreats_Count()
{
  char file[] =
  "tiles {"
  "p,p,p,p,p,"
  "p,p,p,p,p,"
  "p,p,p,p,p,"
  "p,p,p,p,p,"
  "p,p,p,p,p,"
  "p,p,p,p,p,}"
  "units {"
  "--,--,--,--,--,"
  "--,--,--,--,--,"
  "--,--,--,--,--,"
  "--,--,a1,--,--,"
  "--,--,--,--,--,"
  "--,--,--,S2,--,}";
  GTBoard b;
  GTBoard_Init(&b);
  GTBoard_Parse(&b, file);
  GTAIThreats t;
  GTAIThreats_Init(&t);
  GTAIThreats_Count(&t, &b, &b.units[0]);
  mu_assert(t.count == 12, ".count wrong");
  GTAIThreats_Init(&t);
  GTAIThreats_Count(&t, &b, &b.units[1]);
  mu_assert(t.count == 3, ".count wrong");
  return NULL;
}

static char* Test_All()
{
  mu_suite_start();
  mu_run_test(Test_GTAIMove_ToCommand);
  mu_run_test(Test_GTAIMoves_Generate);
  mu_run_test(Test_GTAIMoves_GenerateAll);
  mu_run_test(Test_GTAIThreats_Count);
  return NULL;
}

RUN_TESTS(Test_All)
