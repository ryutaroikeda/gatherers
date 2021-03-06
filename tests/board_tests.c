#include "minunit.h"
#include "board.h"
#include <string.h>

static char* Test_GTBoard_Init()
{
  GTBoard b;
  mu_assert(GTBoard_Init(&b) == 0, "init failed");
  mu_assert(b.board[GTBoard_ValidMin] == GTBoard_Empty, 
    ".board[pos] not empty");
  mu_assert(b.board[GTBoard_InvalidMin] == GTBoard_Invalid,
    ".board[invalidmin] not invalid");
  mu_assert(GTStack_IsEmpty(&b.stack), ".stack not empty");
  mu_assert(!b.didProduceUnit, ".didProduceUnit wrong");
  return NULL;
}

static char* Test_GTBoard_Pos()
{
  int pos = GTBoard_Pos(0, GTBoard_Height - 1);
  mu_assert(pos == GTBoard_ValidMin, "wrong pos");
  pos = GTBoard_Pos(GTBoard_Width - 1, 0);
  mu_assert(pos == GTBoard_InvalidMin -1, "wrong pos");
  return NULL;
}

static char* Test_GTBoard_Rank()
{
  int pos;
  pos = GTBoard_Pos(0, 0);
  mu_assert(GTBoard_Rank(pos) == 0, "wrong rank");
  pos = GTBoard_Pos(GTBoard_Width - 1, GTBoard_Height - 1);
  mu_assert(GTBoard_Rank(pos) == GTBoard_Width - 1, "wrong rank");
  return NULL;
}

static char* Test_GTBoard_File()
{
  int pos;
  pos = GTBoard_Pos(0, 0);
  mu_assert(GTBoard_File(pos) == 0, "wrong file");
  pos = GTBoard_Pos(GTBoard_Width - 1, GTBoard_Height - 1);
  mu_assert(GTBoard_File(pos) == GTBoard_Height - 1, "wrong file");
  return NULL;
}

static char* Test_GTBoard_IsEqual()
{
  GTBoard b;
  GTBoard_Init(&b);
  mu_assert(GTBoard_IsEqual(b, b), "not reflexive");
  GTBoard c;
  GTBoard_Init(&c);
  c.board[0] = 389457;
  mu_assert(!GTBoard_IsEqual(b, c), "unequal are equal");
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

static char* Test_GTBoard_IsRanged()
{
  GTBoard b;
  GTBoard_Init(&b);
  GTBoard_CreateUnit(&b, GTPlayer_None, GTUnitType_Gatherer, GTBoard_ValidMin);
  mu_assert(!GTBoard_IsRanged(&b, 0), "unit not ranged");
  b.units[0].type = GTUnitType_Archer;
  mu_assert(GTBoard_IsRanged(&b, 0), "unit ranged");
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
  mu_assert(!GTBoard_CanMoveUnit(&b, 0, GTDirection_EastEast),
    "can move easteast");
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
  b.units[0].type = GTUnitType_Fortress;
  mu_assert(!GTBoard_CanMoveUnit(&b, 0, GTDirection_South),
   "can move non-moving unit");
  return NULL;
}

static char* Test_GTBoard_CanProduceUnit()
{
  GTBoard b;
  GTBoard_Init(&b);
  b.tiles[GTBoard_ValidMin].type = GTTileType_Iron;
  GTBoard_CreateUnit(&b, GTPlayer_Black, GTUnitType_Gatherer, GTBoard_ValidMin);
  mu_assert(!GTBoard_CanProduceUnit(&b, 0, GTUnitType_Gatherer,
   GTDirection_East), "can produce with no movement");
  GTBoard_ResetUnitMovement(&b, 0);
  mu_assert(!GTBoard_CanProduceUnit(&b, 0, GTUnitType_Spearman, 
   GTDirection_North), "can produce on invalid");
  mu_assert(GTBoard_CanProduceUnit(&b, 0, GTUnitType_Spearman,
   GTDirection_East), "cannot produce spearman");
  mu_assert(!GTBoard_CanProduceUnit(&b, 0, GTUnitType_Archer, 
   GTDirection_East), "can produce archer");
  mu_assert(GTBoard_CanProduceUnit(&b, 0, GTUnitType_Gatherer,
    GTDirection_East), "cannot produce gatherer");
  GTBoard_CreateUnit(&b, GTPlayer_White, GTUnitType_Gatherer,
   GTDirection_PosEast(GTBoard_ValidMin));
  mu_assert(!GTBoard_CanProduceUnit(&b, 0, GTUnitType_Gatherer, 
    GTDirection_East), "can produce on top of unit");
  return NULL;
}

static char* Test_GTBoard_CanRange()
{
  GTBoard b;
  GTBoard_Init(&b);
  GTBoard_CreateUnit(&b, GTPlayer_Black, GTUnitType_Archer, GTBoard_ValidMin);
  mu_assert(!GTBoard_CanRange(&b, 0, GTDirection_East),
    "unit with no movement can range");
  GTBoard_ResetUnitMovement(&b, 0);
  mu_assert(!GTBoard_CanRange(&b, 0, GTDirection_East), "unit can range empty");
  GTBoard_CreateUnit(&b, GTPlayer_Black, GTUnitType_Archer, 
    GTDirection_Pos(GTBoard_ValidMin, GTDirection_EastEast));
  mu_assert(!GTBoard_CanRange(&b, 0, GTDirection_EastEast), "can range friend");
  b.units[0].color = GTPlayer_White;
  mu_assert(GTBoard_CanRange(&b, 0, GTDirection_EastEast), "can't range");
  b.tiles[GTBoard_ValidMin + 1].type = GTTileType_Mountain;
  b.tiles[GTBoard_ValidMin + 1].isRevealed = 1;
  mu_assert(!GTBoard_CanRange(&b, 0, GTDirection_EastEast), 
    "can range over revealed mountain");
  return NULL;
}

static char* Test_GTBoard_CanStay()
{
  GTBoard b;
  GTBoard_Init(&b);
  GTBoard_CreateUnit(&b, GTPlayer_None, GTUnitType_Gatherer, GTBoard_ValidMin);
  mu_assert(!GTBoard_CanStay(&b, 0), "unit with mvmt = 0 can stay");
  GTBoard_ResetUnitMovement(&b, 0);
  mu_assert(GTBoard_CanStay(&b, 0), "unit can't stay");
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
  mu_assert(GTBoard_CreateUnit(&b, GTPlayer_Black, GTUnitType_Gatherer,
   GTBoard_ValidMin) == 0, "unit overflow");
  mu_assert(b.unitId == 1, ".unitId not updated");
  mu_assert(b.board[GTBoard_ValidMin] == 0, ".board not updated");
  mu_assert(b.units[0].type == GTUnitType_Gatherer, "created wrong unit");
  mu_assert(b.units[0].color == GTPlayer_Black, ".color wrong");
  mu_assert(b.units[0].movement == 0, ".movement wrong");
  mu_assert(b.population[GTPlayer_Black][GTUnitType_Gatherer] == 1,
    ".population wrong");
  GTTileType t = b.tiles[GTBoard_ValidMin].type;
  mu_assert(b.resources[GTPlayer_Black][t] == 1, ".resources wrong");
  GTBoard_CreateUnit(&b, GTPlayer_White, GTUnitType_Archer,
   GTDirection_PosEast(GTBoard_InvalidMin));
  t = b.tiles[GTDirection_PosEast(GTBoard_InvalidMin)].type;
  mu_assert(b.resources[GTPlayer_White][t] == 0, ".resources wrong");
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

static char* Test_GTBoard_DeleteUnit()
{
  GTBoard b;
  GTBoard_Init(&b);
  mu_assert(GTBoard_DeleteUnit(&b, 0) == -1, "removed invalid unit");
  GTBoard_CreateUnit(&b, GTPlayer_None, GTUnitType_Gatherer, GTBoard_ValidMin);
  mu_assert(b.population[GTPlayer_None][GTUnitType_Gatherer] == 1,
    "wrong .population");
  mu_assert(GTBoard_DeleteUnit(&b, 0) == 0, "can't remove valid unit");
  mu_assert(b.units[0].life == 0, "wrong .life");
  mu_assert(b.units[0].movement == 0, "wrong .movement");
  mu_assert(b.population[GTPlayer_None][GTUnitType_Gatherer] == 0, 
    "wrong .population");
  GTTileType t = b.tiles[b.units[0].pos].type;
  mu_assert(b.resources[GTPlayer_None][t] == 0, "wrong .resources");
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
  b.tiles[GTBoard_ValidMin].type = GTTileType_Wood;
  b.tiles[GTDirection_PosEast(GTBoard_ValidMin)].type = GTTileType_Horse;
  mu_assert(GTBoard_MoveUnit(&b, 0, GTDirection_None) == -1,
   "moved invalid unit");
  GTBoard_CreateUnit(&b, GTPlayer_Black, GTUnitType_Gatherer, GTBoard_ValidMin);
  GTBoard_ResetUnitMovement(&b, 0);
  mu_assert(GTBoard_MoveUnit(&b, 0, GTDirection_East) == 0,
   "can't move valid unit");
  mu_assert(b.resources[GTPlayer_Black][GTTileType_Wood] == 0,
    "wood wrong");
  mu_assert(b.resources[GTPlayer_Black][GTTileType_Horse] == 1,
    "horse wrong");
  GTBoard_CreateUnit(&b, GTPlayer_White, GTUnitType_Cavalry, GTBoard_ValidMin);
  GTBoard_ResetUnitMovement(&b, 1);
  mu_assert(GTBoard_MoveUnit(&b, 1, GTDirection_East) == 0,
    "can't move valid unit");
  mu_assert(!GTBoard_IsValidUnit(&b, 0), "didn't kill unit");
  mu_assert(b.units[1].pos == GTDirection_PosEast(GTBoard_ValidMin), 
    "wrong .pos");
  mu_assert(b.units[1].movement == -1, ".movement wrong");
  mu_assert(GTBoard_IsRevealed(&b, GTDirection_PosEast(GTBoard_ValidMin)),
    "new pos not revealed");
  mu_assert(GTBoard_IsEmpty(&b, GTBoard_ValidMin), "old pos not empty");
  return NULL;
}

static char* Test_GTBoard_ProduceUnit()
{
  GTBoard b;
  GTBoard_Init(&b);
  b.tiles[GTBoard_ValidMin].type = GTTileType_Wood;
  GTBoard_CreateUnit(&b, GTPlayer_Black, GTUnitType_Gatherer, GTBoard_ValidMin);
  GTBoard_ResetUnitMovement(&b, 0);
  mu_assert(
    GTBoard_ProduceUnit(&b, 0, GTUnitType_Archer, GTDirection_East) == 0,
    "couldn't produce archer");
  mu_assert(b.didProduceUnit == 1, ".didProduceUnit wrong");
  mu_assert(GTBoard_IsValidUnit(&b, 1), "no valid unit produced");
  GTUnit* u = &b.units[0];
  mu_assert(u->movement == 0, ".movement wrong");
  GTBoard_ResetUnitMovement(&b, 0);
  mu_assert(!GTBoard_CanProduceUnit(&b, 0, GTUnitType_Archer,
   GTDirection_South), "can produce archer");
  return NULL;
}

static char* Test_GTBoard_Range()
{
  GTBoard b;
  GTBoard_Init(&b);
  int east = GTDirection_PosEast(GTBoard_ValidMin);
  int easteast = GTDirection_Pos(GTBoard_ValidMin, GTDirection_EastEast);
  b.tiles[east].type = GTTileType_Mountain;
  mu_assert(GTBoard_Range(&b, 0, GTDirection_South) == -1,
   "invalid unit ranged");
  GTBoard_CreateUnit(&b, GTPlayer_Black, GTUnitType_Archer, GTBoard_ValidMin);
  GTBoard_CreateUnit(&b, GTPlayer_White, GTUnitType_Archer, easteast);
  GTBoard_ResetUnitMovement(&b, 0);
  mu_assert(GTBoard_Range(&b, 0, GTDirection_EastEast) == 0,
    "range should not error even with mountain");
  mu_assert(GTBoard_IsUnit(&b, easteast), "mountain ignored");
  mu_assert(b.tiles[east].isRevealed, "mountain not revealed");
  b.tiles[east].type = GTTileType_Plain;
  GTBoard_ResetUnitMovement(&b, 0);
  mu_assert(GTBoard_Range(&b, 0, GTDirection_EastEast) == 0, "range failed");
  mu_assert(GTBoard_IsEmpty(&b, easteast), "board not empty");
  mu_assert(b.units[1].life == 0, "unit not damaged");
  return NULL;
}

static char* Test_GTBoard_Stay()
{
  GTBoard b;
  GTBoard_Init(&b);
  GTBoard_CreateUnit(&b, GTPlayer_None, GTUnitType_Gatherer, GTBoard_ValidMin);
  GTBoard_ResetUnitMovement(&b, 0);
  mu_assert(b.units[0].movement == 1, ".movement wrong");
  GTBoard_Stay(&b, 0);
  mu_assert(b.units[0].movement == 0, ".movement wrong");
  return NULL;
}

static char* Test_GTBoard_UndoPlay()
{
  GTBoard b;
  GTBoard_Init(&b);
  mu_assert(GTBoard_UndoPlay(&b) == -1, "undoing with nothing to undo");
  GTBoard_CreateUnit(&b, GTPlayer_Black, GTUnitType_Gatherer, GTBoard_ValidMin);
  GTBoard_ResetUnitMovement(&b, 0);
  GTBoard c = b;
  mu_assert(GTBoard_MoveUnit(&b, 0, GTDirection_East) == 0, "can't move unit");
  mu_assert(GTBoard_UndoPlay(&b) == 0, "undo failed");
  mu_assert(GTBoard_IsEqual(b, c), "didn't undo move");
  mu_assert(
    GTBoard_ProduceUnit(&b, 0, GTUnitType_Gatherer, GTDirection_East) == 0,
    "produce failed");
  mu_assert(GTBoard_UndoPlay(&b) == 0, "undo failed");
  mu_assert(GTBoard_IsEqual(b, c), "didn't undo produce");
  int easteast = GTDirection_Pos(GTBoard_ValidMin, GTDirection_EastEast);
  GTBoard_CreateUnit(&b, GTPlayer_White, GTUnitType_Archer, easteast);
  GTBoard_ResetUnitMovement(&b, 1);
  c = b;
  mu_assert(GTBoard_Range(&b, 1, GTDirection_WestWest) == 0, "range failed");
  mu_assert(GTBoard_UndoPlay(&b) == 0, "undo failed");
  mu_assert(GTBoard_IsEqual(b, c), "didn't undo range");
  return NULL;
}

static char* Test_GTBoard_EndTurn()
{
  GTBoard b;
  GTBoard_Init(&b);
  mu_assert(GTBoard_EndTurn(&b) == 0, "endturn failed");
  return NULL;
}

static char* Test_GTBoard_ParseUnit()
{
  GTBoard b;
  GTBoard_Init(&b);
  mu_assert(GTBoard_ParseUnit(&b, "W5", GTBoard_ValidMin) == -1,
    "parsed invalid token");
  mu_assert(GTBoard_ParseUnit(&b, "g1", GTBoard_ValidMin) == 0,
   "parse failed");
  mu_assert(GTBoard_IsUnit(&b, GTBoard_ValidMin), "no unit created");
  GTUnit* u = &b.units[b.board[GTBoard_ValidMin]];
  mu_assert(u->color == GTPlayer_Black, ".color wrong");
  mu_assert(u->type == GTUnitType_Gatherer, ".type wrong");
  mu_assert(u->life == 1, ".life wrong");
  return NULL;
}

static char* Test_GTBoard_ParseUnits()
{
  GTBoard b;
  GTBoard_Init(&b);
  char units1[] = "we can fly if only we can believe it a dream";
  char units2[] =
  "g1,a1,c2,s3,f4,"
  "G1,A1,C2,S3,F4,"
  "--,--,--,--,--,"
  " \r \n\t --,g4,S9,aa,fk,"
  "--,--,--,--,--,"
  "--,--,--,--,--,";
  mu_assert(GTBoard_ParseUnits(&b, units1) == -1, "parsed bad string");
  mu_assert(GTBoard_ParseUnits(&b, units2) == 0, "parse failed");
  mu_assert(b.population[GTPlayer_Black][GTUnitType_Gatherer] == 2,
    ".population gatherer wrong");
  mu_assert(b.population[GTPlayer_Black][GTUnitType_Archer] == 2,
    ".population archer wrong");
  mu_assert(b.population[GTPlayer_Black][GTUnitType_Cavalry] == 1,
    ".population cavalry wrong");
  mu_assert(b.population[GTPlayer_Black][GTUnitType_Spearman] == 1,
    ".population spearman wrong");
  mu_assert(b.population[GTPlayer_Black][GTUnitType_Fortress] == 2,
    ".population fortress wrong");
  mu_assert(b.population[GTPlayer_White][GTUnitType_Spearman] == 2,
    ".population Spearman wrong");
  return NULL;
}

static char* Test_GTBoard_ParseTile()
{
  GTBoard b;
  GTBoard_Init(&b);
  char tok1 = 'y';
  char tok2 = 'w';
  mu_assert(GTBoard_ParseTile(&b, tok1, GTBoard_ValidMin) == -1,
   "parsed bad token");
  mu_assert(GTBoard_ParseTile(&b, tok2, GTBoard_ValidMin) == 0,
    "parse failed");
  mu_assert(b.tiles[GTBoard_ValidMin].type == GTTileType_Wood, ".type wrong");
  mu_assert(b.tiles[GTBoard_ValidMin].isRevealed == 0, ".isRevealed wrong");
  return NULL;
}

static char* Test_GTBoard_ParseTiles()
{
  GTBoard b;
  GTBoard_Init(&b);
  char tiles1[] = "they will say: how his hair is growing thin!";
  char tiles2[] =
  "m, m, m, m, m,"
  "l, l, l, l, l,"
  "i, i, i, i, i,"
  "h, h, h, h, h,"
  "p, p, p, p, p,"
  "w, w, w, w, w";
  mu_assert(GTBoard_ParseTiles(&b, tiles1) == -1, "parsed bad string");
  mu_assert(GTBoard_ParseTiles(&b, tiles2) == 0, "parse failed");
  int pos = GTBoard_ValidMin;
  mu_assert(b.tiles[pos].type == GTTileType_Mountain, ".tiles wrong");
  pos = GTDirection_Pos(pos, GTDirection_SouthEast);
  mu_assert(b.tiles[pos].type == GTTileType_Lake, ".tiles wrong");
  pos = GTDirection_Pos(pos, GTDirection_SouthEast);
  mu_assert(b.tiles[pos].type == GTTileType_Iron, ".tiles wrong");
  pos = GTDirection_Pos(pos, GTDirection_SouthEast);
  mu_assert(b.tiles[pos].type == GTTileType_Horse, ".tiles wrong");
  pos = GTDirection_Pos(pos, GTDirection_SouthEast);
  mu_assert(b.tiles[pos].type == GTTileType_Plain, ".tiles wrong");
  pos = GTDirection_Pos(pos, GTDirection_South);
  mu_assert(b.tiles[pos].type == GTTileType_Wood, ".tiles wrong");
  return NULL;
}

static char* Test_GTBoard_Parse()
{
  GTBoard b;
  GTBoard_Init(&b);
  char file1[] = "look upon my work, ye mighty, and despair!";
  char file2[] =
  "\ntiles {"
  "m, m, m, m, m,"
  "l, l, l, l, l,"
  "i, i, i, i, i,"
  "h, h, h, h, h,"
  "p, p, p, p, p,"
  "w, w, w, w, w,}"
  "\nunits\t{\n"
  "--,g1,--,A1,--,\n"
  "--,--,--,--,f9,\n"
  "C2,--,--,--,--,\n"
  "g1,g1,g1,g1,G1,"
  "s3,s3,s3,s3,S3,"
  "--,--,--,--,--,\r\n\t}";
  char file3[] = "options {}";
  char file4[] = "options{}";
  mu_assert(GTBoard_Parse(&b, file1) == -1, "parsed bad string");
  mu_assert(GTBoard_Parse(&b, file2) == 0, "parse failed");
  mu_assert(GTBoard_Parse(&b, file3) == 0, "parse failed");
  mu_assert(GTBoard_Parse(&b, file4) == -1, "parsed bad string");
  return NULL;
}

static char* Test_All()
{
  mu_suite_start();
  mu_run_test(Test_GTBoard_Init);
  mu_run_test(Test_GTBoard_Pos);
  mu_run_test(Test_GTBoard_Rank);
  mu_run_test(Test_GTBoard_File);
  mu_run_test(Test_GTBoard_IsEqual);
  mu_run_test(Test_GTBoard_IsValid);
  mu_run_test(Test_GTBoard_IsEmpty);
  mu_run_test(Test_GTBoard_IsUnit);
  mu_run_test(Test_GTBoard_IsRevealed);
  mu_run_test(Test_GTBoard_IsValidUnit);
  mu_run_test(Test_GTBoard_IsRanged);
  mu_run_test(Test_GTBoard_CanMoveUnit);
  mu_run_test(Test_GTBoard_CanProduceUnit);
  mu_run_test(Test_GTBoard_CanRange);
  mu_run_test(Test_GTBoard_CanStay);
  mu_run_test(Test_GTBoard_RevealTile);
  mu_run_test(Test_GTBoard_CreateUnit);
  mu_run_test(Test_GTBoard_ResetUnitMovement);
  mu_run_test(Test_GTBoard_DeleteUnit);
  mu_run_test(Test_GTBoard_DamageUnit);
  mu_run_test(Test_GTBoard_MoveUnit);
  mu_run_test(Test_GTBoard_Range);
  mu_run_test(Test_GTBoard_ProduceUnit);
  mu_run_test(Test_GTBoard_Stay);
  mu_run_test(Test_GTBoard_UndoPlay);
  mu_run_test(Test_GTBoard_EndTurn);
  mu_run_test(Test_GTBoard_ParseUnit);
  mu_run_test(Test_GTBoard_ParseUnits);
  mu_run_test(Test_GTBoard_ParseTile);
  mu_run_test(Test_GTBoard_ParseTiles);
  mu_run_test(Test_GTBoard_Parse);
  return NULL;
}

RUN_TESTS(Test_All)
