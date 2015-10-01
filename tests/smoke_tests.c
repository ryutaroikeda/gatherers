#include "minunit.h"
#include "board.h"
#include "cmdline.h"
#include "game.h"

#define Test_CharGet(i) \
static char Test_CharGet ## i(void) \
{                                   \
  static char* ptr = input ## i;    \
  return *(ptr++);                  \
}

#define Test_GetCommand(i)                     \
Test_CharGet(i)                                \
static int Test_GetCommand ## i (GTCommand* c) \
{                                              \
  return GTCommand_Get(c, &Test_CharGet ## i); \
}

static char input1[] = "done\nmv c1 n\ndone\nmv c6 s\ndone\npd c2 n g\nexit\n";
Test_GetCommand(1)

static char* Test_1()
{
  GTBoard b;
  GTBoard_Init(&b);
  int pos1 = GTBoard_Pos(2, 5);
  int pos2 = GTBoard_Pos(2, 0);
  GTBoard_CreateUnit(&b, GTPlayer_Black, GTUnitType_Gatherer, pos1);
  GTBoard_CreateUnit(&b, GTPlayer_White, GTUnitType_Gatherer, pos2);
  GTGame g;
  GTGame_Init(&g, &b);
  g.interface[GTPlayer_Black] = &Test_GetCommand1;
  g.interface[GTPlayer_White] = &Test_GetCommand1;
  GTGame_Play(&g);
  mu_assert(b.units[0].pos == GTDirection_PosSouth(pos1), ".pos wrong");
  mu_assert(b.units[1].pos == GTDirection_PosNorth(pos2), ".pos wrong");
  mu_assert(GTBoard_IsValidUnit(&b, 2), "unit not created");
  mu_assert(b.units[2].pos == GTDirection_Pos(pos2, GTDirection_NorthNorth),
    ".pos wrong");
  return NULL;
}

static char input2[] = "rg c4 ss\nexit\n";
Test_GetCommand(2)

static char* Test_2()
{
  char file[] =
  "tiles {"
  " w, m, h, i, w,"
  " p, h, w, m, h,"
  " m, i, p, m, i,"
  " i, m, p, i, m,"
  " h, m, w, h, p,"
  " w, i, h, m, w, }"
  "units {"
  "--,--,g1,--,--,"
  "--,--,--,--,--,"
  "--,--,a1,--,--,"
  "--,--,S3,--,--,"
  "--,--,G1,--,--,"
  "--,--,--,--,--, }";
  GTBoard b;
  GTBoard_Init(&b);
  GTBoard_Parse(&b, file);
  GTGame g;
  GTGame_Init(&g, &b);
  g.interface[GTPlayer_Black] = &Test_GetCommand2;
  g.interface[GTPlayer_White] = &Test_GetCommand2;
  GTGame_Play(&g);
  mu_assert(GTBoard_IsEmpty(&b, GTBoard_Pos(2, 1)), "range failed");
  mu_assert(GTGame_IsEnd(&g), "game not ended");
  return NULL;
}

static char input3[] = "st c4\nmv c4 n\nexit\n";
Test_GetCommand(3)

static char* Test_3()
{
  char file[] =
  "tiles {"
  " w, m, h, i, w,"
  " p, h, w, m, h,"
  " m, i, p, m, i,"
  " i, m, p, i, m,"
  " h, m, w, h, p,"
  " w, i, h, m, w, }"
  "units {"
  "--,--,g1,--,--,"
  "--,--,--,--,--,"
  "--,--,a1,--,--,"
  "--,--,S3,--,--,"
  "--,--,G1,--,--,"
  "--,--,--,--,--, }";
  GTBoard b;
  GTBoard_Init(&b);
  GTBoard_Parse(&b, file);
  GTGame g;
  GTGame_Init(&g, &b);
  g.interface[GTPlayer_Black] = &Test_GetCommand3;
  g.interface[GTPlayer_White] = &Test_GetCommand3;
  GTGame_Play(&g);
  mu_assert(GTBoard_IsEmpty(&b, GTBoard_Pos(2, 4)), "stay didn't prevent move");
  return NULL;
}

static char* Test_All()
{
  mu_suite_start();
  mu_run_test(Test_1);
  mu_run_test(Test_2);
  mu_run_test(Test_3);
  return NULL;
}

RUN_TESTS(Test_All)
