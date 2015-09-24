#include "gatherers.h"
#include "minunit.h"

char* Test_GTBoard_CreateUnit()
{
  GTBoard b;
  GTBoard_Init(&b);
  GTBoard_CreateUnit(&b, GTUnitType_Archer, GTBoard_ValidMin);
  mu_assert(b.unitId == 1, ".unitId not updated");
  mu_assert(b.board[GTBoard_ValidMin] == 0, ".board not updated");
  mu_assert(b.units[0].type == GTUnitType_Archer, "created wrong unit");
  return NULL;
}

char* Test_All()
{
  mu_suite_start();
  mu_run_test(Test_GTBoard_CreateUnit);
  return NULL;
}

RUN_TESTS(Test_All)
