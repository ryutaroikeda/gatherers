#include "minunit.h"
#include "board.h"
#include "cmdline.h"
#include "game.h"

static char input1[] = "dn\nc1 mv n\ndn\nc6 mv s\ndn\nc2 pd g n\nexit";

static char Test_CharGetter1(void)
{
  static char* ptr = input1;
  return *(ptr++);
}

static int Test_GetCommand1(GTCommand* c)
{
  return GTCommand_Get(c, &Test_CharGetter1);
}

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
  GTGame_PlayExplicit(&g, &Test_GetCommand1);
  mu_assert(b.units[0].pos == GTDirection_PosSouth(pos1), ".pos wrong");
  mu_assert(b.units[1].pos == GTDirection_PosNorth(pos2), ".pos wrong");
  mu_assert(GTBoard_IsValidUnit(&b, 2), "unit not created");
  mu_assert(b.units[2].pos == GTDirection_Pos(pos2, GTDirection_NorthNorth),
    ".pos wrong");
  return NULL;
}

static char* Test_All()
{
  mu_suite_start();
  mu_run_test(Test_1);
  return NULL;
}

RUN_TESTS(Test_All)
