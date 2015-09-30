#ifndef _GTBOARD_H_
#define _GTBOARD_H_
#include "util.h"
#include "types.h"
#include "stack.h"
#include "io.h"

//
// Unless otherwise stated, the functions below return 0 on success and a
// negative integer on failure.
//

extern const int GTDirection_Direction[GTDirection_Size];

#define GTDirection_PosNorth(pos) \
((pos) + GTDirection_Direction[GTDirection_North])
#define GTDirection_PosEast(pos) \
((pos) + GTDirection_Direction[GTDirection_East])
#define GTDirection_PosSouth(pos) \
((pos) + GTDirection_Direction[GTDirection_South])
#define GTDirection_PosWest(pos) \
((pos) + GTDirection_Direction[GTDirection_West])
#define GTDirection_Pos(pos, d) \
((pos) + GTDirection_Direction[d])

enum GTBoardError
{
  GTBoardError_None,
  GTBoardError_CannotMoveUnit,
  GTBoardError_FailedUndoPlay,
  GTBoardError_Size
};
enum_type(GTBoardError);

enum GTBoardFileToken
{
  GTBoardFileToken_None,
  GTBoardFileToken_Units,
  GTBoardFileToken_Tiles,
  GTBoardFileToken_End,
  GTBoardFileToken_Size
};
enum_type(GTBoardFileToken);

enum
{
  GTBoard_Width = 5,
  GTBoard_Height = 6,
  GTBoard_WidthMax = 1 + GTBoard_Width + 1,
  GTBoard_HeightMax = 1 + GTBoard_Height + 1,
  GTBoard_Size = GTBoard_WidthMax * GTBoard_HeightMax,
  GTBoard_ValidMin = GTBoard_WidthMax + 1,
  GTBoard_InvalidMin = GTBoard_Size - GTBoard_WidthMax - 1,
  // for .board
  GTBoard_Empty = -1,
  GTBoard_Invalid = -2,

  // do not recycle units to avoid having to search
  GTBoard_UnitSize = 1000,
  GTBoard_StackSize = 10000
};

struct GTBoard
{
  // store state of units
  GTUnit units[GTBoard_UnitSize];
  // store the tile of each position
  GTTile tiles[GTBoard_Size];
  // store changes to the game state
  GTStackEntry entries[GTBoard_StackSize];
  GTStack stack;
  // store the index of the unit on each position
  int board[GTBoard_Size];
  // number of units for each unit type
  int population[GTPlayer_Size][GTUnitType_Size];
  // number of resources acquired
  int resources[GTPlayer_Size][GTTileType_Size];
  // index of next free unit
  int unitId;
  // flag
  int didProduceProducer;
  int didProduceUnit;
  // number of turns taken
  int turn;
  // error number
  GTBoardError err;
};
struct_type(GTBoard);

#define GTBoard_Pos(rank, file) \
(GTBoard_ValidMin + (rank) + ((GTBoard_Height - 1 - (file)) * GTBoard_WidthMax))

int GTBoard_Init(GTBoard* b);
// return 1 if the boards are equal, except for err, stack, and entries
int GTBoard_IsEqual(GTBoard b, GTBoard c);
// return 1 if .board[pos] is valid
int GTBoard_IsValid(const GTBoard* b, int pos);
// return 1 if .board[pos] is empty
int GTBoard_IsEmpty(const GTBoard* b, int pos);
// return 1 if .board[pos] is a unit
int GTBoard_IsUnit(const GTBoard* b, int pos);
// return 1 if .tiles[pos] is revealed
int GTBoard_IsRevealed(const GTBoard* b, int pos);
// return 1 if unit is in .board at the correct pos
int GTBoard_IsValidUnit(const GTBoard* b, int unit);
// return 1 if unit can move d or attack d
int GTBoard_CanMoveUnit(const GTBoard* b, int unit, GTDirection d);

int
GTBoard_CanProduceUnit(const GTBoard* b, int unit, GTUnitType t, GTDirection d);
// return 1 if unit can perform a ranged attack on d
int GTBoard_CanRange(const GTBoard* b, int unit, GTDirection d);

int GTBoard_RevealTile(GTBoard* b, int pos);

int GTBoard_CreateUnit(GTBoard* b, GTPlayer p, GTUnitType t, int pos);

int GTBoard_ResetUnitMovement(GTBoard* b, int unit);

int GTBoard_DeleteUnit(GTBoard* b, int unit);

int GTBoard_DamageUnit(GTBoard* b, int unit, int damage);

int GTBoard_MoveUnit(GTBoard* b, int unit, GTDirection d);

int GTBoard_ProduceUnit(GTBoard* b, int unit, GTUnitType t, GTDirection d);

int GTBoard_Range(GTBoard* b, int unit, GTDirection d);

int GTBoard_UndoPlay(GTBoard* b);

int GTBoard_EndTurn(GTBoard* b);
// parse tok and create unit at pos
int GTBoard_ParseUnit(GTBoard* b, char* tok, int pos);
// parse comma separated unit tokens
int GTBoard_ParseUnits(GTBoard* b, char* s);

int GTBoard_Parse_Tiles(GTBoard* b, char* s);

int GTBoard_Parse(GTBoard* b, char* s);

int GTBoard_Get(GTBoard* b, GTCharGetter cg);
// print board to stdout
int GTBoard_Print(const GTBoard* b);

#endif
