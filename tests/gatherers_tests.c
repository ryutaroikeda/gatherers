#include "minunit.h"
#include "gatherers.h"
#include <string.h>

static char* Test_GTBoard_Init()
{
  GTBoard b;
  mu_assert(GTBoard_Init(&b) == 0, "init failed");
  mu_assert(b.board[GTBoard_ValidMin] == GTBoard_Empty, 
    ".board[pos] not empty");
  mu_assert(GTStack_IsEmpty(&(b.stack)), ".stack not empty");
  return NULL;
}

static char* Test_GTBoard_IsValid()
{
  GTBoard b;
  GTBoard_Init(&b);
  mu_assert(GTBoard_IsValid(&b, GTBoard_ValidMin), "valid not valid");
  mu_assert(!GTBoard_IsValid(&b, GTBoard_InvalidMin), "invalid is valid");
  return NULL;
}

static char* Test_GTBoard_IsEmpty()
{
  GTBoard b;
  GTBoard_Init(&b);
  mu_assert(GTBoard_IsEmpty(&b, GTBoard_ValidMin), "empty not empty");
  mu_assert(GTBoard_CreateUnit(&b, GTPlayer_None, GTUnitType_None,
   GTBoard_ValidMin) == 0, "unit overflow");
  mu_assert(!GTBoard_IsEmpty(&b, GTBoard_ValidMin), "unit is empty");
  mu_assert(!GTBoard_IsEmpty(&b, GTBoard_InvalidMin), "invalid is empty");
  return NULL;
}

static char* Test_GTBoard_IsUnit()
{
  GTBoard b;
  GTBoard_Init(&b);
  mu_assert(!GTBoard_IsUnit(&b, GTBoard_ValidMin), "empty is unit");
  mu_assert(GTBoard_CreateUnit(&b, GTPlayer_None, GTUnitType_None,
   GTBoard_ValidMin) == 0, "unit overflow");
  mu_assert(GTBoard_IsUnit(&b, GTBoard_ValidMin), "unit not unit");
  mu_assert(!GTBoard_IsUnit(&b, GTBoard_InvalidMin), "invalid is unit");
  return NULL;
}

static char* Test_GTBoard_IsRevealed()
{
  GTBoard b;
  GTBoard_Init(&b);
  mu_assert(!GTBoard_IsRevealed(&b, GTBoard_ValidMin), "invisible is visible");
  b.tiles[GTBoard_ValidMin].isRevealed = 1;
  mu_assert(GTBoard_IsRevealed(&b, GTBoard_ValidMin), "visible is invisible");
  return NULL;
}

static char* Test_GTBoard_IsValidUnit()
{
  GTBoard b;
  GTBoard_Init(&b);
  mu_assert(!GTBoard_IsValidUnit(&b, 0), "invalid is valid");
  GTBoard_CreateUnit(&b, GTPlayer_None, GTUnitType_None, GTBoard_ValidMin);
  mu_assert(GTBoard_IsValidUnit(&b, 0), "valid is invalid");
  return NULL;
}

static char* Test_GTBoard_CanMoveUnit()
{
  GTBoard b;
  GTBoard_Init(&b);
  mu_assert(!GTBoard_CanMoveUnit(&b, 0, GTDirection_None),
   "invalid unit can move");
  GTBoard_CreateUnit(&b, GTPlayer_Black, GTUnitType_Gatherer, GTBoard_ValidMin);
  GTBoard_ResetUnitMovement(&b, 0);
  mu_assert(GTBoard_CanMoveUnit(&b, 0, GTDirection_None), 
    "can't move to valid");
  mu_assert(!GTBoard_CanMoveUnit(&b, 0, GTDirection_North),
   "can move to invalid");
  mu_assert(GTBoard_CanMoveUnit(&b, 0, GTDirection_East),
   "can't move to valid");
  mu_assert(GTBoard_CanMoveUnit(&b, 0, GTDirection_South),
   "can't move to valid");
  mu_assert(!GTBoard_CanMoveUnit(&b, 0, GTDirection_West),
   "can move to invalid");
  GTBoard_CreateUnit(&b, GTPlayer_White, GTUnitType_None,
    GTDirection_PosEast(GTBoard_ValidMin));
  mu_assert(!GTBoard_CanMoveUnit(&b, 0, GTDirection_East),
   "non-attacking unit can attack opposing unit");
  b.units[0].type = GTUnitType_Cavalry;
  mu_assert(GTBoard_CanMoveUnit(&b, 0, GTDirection_East),
   "attacking unit can't attack opposing unit");
  b.units[0].color = GTPlayer_White;
  mu_assert(!GTBoard_CanMoveUnit(&b, 0, GTDirection_East),
    "attacking unit can move on friendly unit");
  b.units[0].type = GTUnitType_Castle;
  mu_assert(!GTBoard_CanMoveUnit(&b, 0, GTDirection_South),
   "can move non-moving unit");
  return NULL;
}

static char* Test_GTBoard_RevealTile()
{
  GTBoard b;
  GTBoard_Init(&b);
  mu_assert(GTBoard_RevealTile(&b, GTBoard_ValidMin) == 0, "reveal failed");
  mu_assert(b.tiles[GTBoard_ValidMin].isRevealed == 1, ".isRevealed wrong");
  return NULL;  
}

static char* Test_GTBoard_CreateUnit()
{
  GTBoard b;
  GTBoard_Init(&b);
  mu_assert(GTBoard_CreateUnit(&b, GTPlayer_None, GTUnitType_None,
   GTBoard_InvalidMin) == -1, "create unit at invalid");
  mu_assert(GTBoard_CreateUnit(&b, GTPlayer_Black, GTUnitType_Archer,
   GTBoard_ValidMin) == 0, "unit overflow");
  mu_assert(b.unitId == 1, ".unitId not updated");
  mu_assert(b.board[GTBoard_ValidMin] == 0, ".board not updated");
  mu_assert(b.units[0].type == GTUnitType_Archer, "created wrong unit");
  mu_assert(b.units[0].color == GTPlayer_Black, ".color wrong");
  mu_assert(b.units[0].movement == 0, ".movement wrong");
  int i;
  for(i = 0; i < GTBoard_UnitSize - 1; i++)
  {
    mu_assert(GTBoard_CreateUnit(&b, GTPlayer_Black, GTUnitType_Archer,
     GTBoard_ValidMin) == 0, "unit overflow");
  }
  mu_assert(GTBoard_CreateUnit(&b, GTPlayer_Black, GTUnitType_Archer,
   GTBoard_ValidMin) == -1, "undetected overflow");
  return NULL;
}

static char* Test_GTBoard_ResetUnitMovement()
{
  GTBoard b;
  GTBoard_Init(&b);
  GTBoard_CreateUnit(&b, GTPlayer_None, GTUnitType_Cavalry, GTBoard_ValidMin);
  mu_assert(GTBoard_ResetUnitMovement(&b, 0) == 0, "reset failed");
  mu_assert(b.units[0].movement == 2, ".movement wrong");
  return NULL;
}

static char* Test_GTBoard_RemoveUnit()
{
  GTBoard b;
  GTBoard_Init(&b);
  mu_assert(GTBoard_RemoveUnit(&b, 0) == -1, "removed invalid unit");
  GTBoard_CreateUnit(&b, GTPlayer_None, GTUnitType_None, GTBoard_ValidMin);
  mu_assert(GTBoard_RemoveUnit(&b, 0) == 0, "can't remove valid unit");
  mu_assert(b.units[0].life == 0, "wrong .life");
  mu_assert(b.units[0].movement == 0, "wrong .movement");
  return NULL;
}

static char* Test_GTBoard_DamageUnit()
{
  GTBoard b;
  GTBoard_Init(&b);
  mu_assert(GTBoard_DamageUnit(&b, 0, 1) == -1, "damaged invalid unit");
  GTBoard_CreateUnit(&b, GTPlayer_None, GTUnitType_Cavalry, GTBoard_ValidMin);
  mu_assert(GTBoard_DamageUnit(&b, 0, 1) == 0, "can't damage valid unit");
  mu_assert(GTBoard_IsValidUnit(&b, 0), "live unit removed");
  mu_assert(GTBoard_DamageUnit(&b, 0, 1) == 0, "can't damage valid unit");
  mu_assert(!GTBoard_IsValidUnit(&b, 0), "dead unit not removed");
  return NULL;
}

static char* Test_GTBoard_MoveUnit()
{
  GTBoard b;
  GTBoard_Init(&b);
  mu_assert(GTBoard_MoveUnit(&b, 0, GTDirection_None) == -1,
   "moved invalid unit");
  GTBoard_CreateUnit(&b, GTPlayer_Black, GTUnitType_Gatherer, GTBoard_ValidMin);
  GTBoard_ResetUnitMovement(&b, 0);
  mu_assert(GTBoard_MoveUnit(&b, 0, GTDirection_East) == 0,
   "can't move valid unit");
  GTBoard_CreateUnit(&b, GTPlayer_White, GTUnitType_Cavalry, GTBoard_ValidMin);
  GTBoard_ResetUnitMovement(&b, 1);
  mu_assert(GTBoard_MoveUnit(&b, 1, GTDirection_East) == 0,
    "can't move valid unit");
  mu_assert(!GTBoard_IsValidUnit(&b, 0), "didn't kill unit");
  mu_assert(b.units[1].pos == GTDirection_PosEast(GTBoard_ValidMin), 
    "wrong .pos");
  mu_assert(b.units[1].movement == 0, ".movement too high");
  mu_assert(GTBoard_IsRevealed(&b, GTDirection_PosEast(GTBoard_ValidMin)),
    "new pos not revealed");
  mu_assert(GTBoard_IsEmpty(&b, GTBoard_ValidMin), "old pos not empty");
  return NULL;
}

static char* Test_GTBoard_UndoPlay()
{
  GTBoard b;
  GTBoard_Init(&b);
  GTBoard c = b;
  mu_assert(GTBoard_UndoPlay(&b) == -1, "undoing with nothing to undo");
  GTStack_BeginPlay(&(b.stack));
  mu_assert(GTBoard_UndoPlay(&b) == 0, "failed undo");
  b.err = c.err;
  b.stack.err = c.stack.err;
  mu_assert(memcmp(&b, &c, sizeof(GTBoard)), "undo didn't restore board");
  GTBoard_CreateUnit(&b, GTPlayer_Black, GTUnitType_Gatherer, GTBoard_ValidMin);
  GTBoard_ResetUnitMovement(&b, 0);
  GTBoard d = b;
  mu_assert(GTBoard_MoveUnit(&b, 0, GTDirection_East) == 0, "can't move unit");
  mu_assert(GTBoard_UndoPlay(&b) == 0, "undo failed");
  b.err = d.err;
  b.stack.err = d.stack.err;
  // popping and purging still keep entries on the stack
  memcpy(b.entries, d.entries, sizeof(b.entries));
  mu_assert(memcmp(&b, &d, sizeof(GTBoard)) == 0, "undo didn't restore board");
  return NULL;
}

static char* Test_All()
{
  mu_suite_start();
  mu_run_test(Test_GTBoard_Init);
  mu_run_test(Test_GTBoard_IsValid);
  mu_run_test(Test_GTBoard_IsEmpty);
  mu_run_test(Test_GTBoard_IsUnit);
  mu_run_test(Test_GTBoard_IsRevealed);
  mu_run_test(Test_GTBoard_IsValidUnit);
  mu_run_test(Test_GTBoard_CanMoveUnit);
  mu_run_test(Test_GTBoard_RevealTile);
  mu_run_test(Test_GTBoard_CreateUnit);
  mu_run_test(Test_GTBoard_ResetUnitMovement);
  mu_run_test(Test_GTBoard_RemoveUnit);
  mu_run_test(Test_GTBoard_DamageUnit);
  mu_run_test(Test_GTBoard_MoveUnit);
  mu_run_test(Test_GTBoard_UndoPlay);
  return NULL;
}

RUN_TESTS(Test_All)
