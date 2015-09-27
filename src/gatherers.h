#ifndef __gatherers_h__
#define __gatherers_h__
#include "util.h"
#include "stack.h"

//
// Unless otherwise stated, the functions below return 0 on success and a
// negative integer on failure.
//

enum GTTileType
{
  GTTileType_None,
  GTTileType_Plains,
  GTTileType_Wood,
  GTTileType_Horse,
  GTTileType_Iron,
  GTTileType_Lake,
  GTTileType_Mountain,
  GTTileType_Size
};
enum_type(GTTileType);

struct GTTile
{
  GTTileType type;
  int isRevealed;
};
struct_type(GTTile);

enum GTPlayer
{
  GTPlayer_None,
  GTPlayer_Black,
  GTPlayer_White,
  GTPlayer_Size
};
enum_type(GTPlayer);

// return 1 if black and white
int GTPlayer_IsOpposite(GTPlayer p, GTPlayer q);

enum GTUnitType
{
  GTUnitType_None,
  GTUnitType_Gatherer,
  GTUnitType_Archer,
  GTUnitType_Cavalry,
  GTUnitType_Spearman,
  GTUnitType_Castle,
  GTUnitType_Size
};
enum_type(GTUnitType);

struct GTUnit
{
  GTPlayer color;
  GTUnitType type;
  int pos;
  int life;
  int movement;
};
struct_type(GTUnit);

enum GTDirection
{
  GTDirection_None,
  GTDirection_North,
  GTDirection_East,
  GTDirection_South,
  GTDirection_West,
  GTDirection_Size
};
enum_type(GTDirection);

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
  int didProduceGatherer;
  // number of turns taken
  int turn;
  // error number
  GTBoardError err;
};
struct_type(GTBoard);

int GTBoard_Init(GTBoard* b);
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

int GTBoard_CanProduceUnit(const GTBoard* b, int unit, GTDirection d);
// return tiles[pos].tile
// GTTileType GTBoard_GetTileType(const GTBoard* b, int pos);

// GTPlayer GTBoard_GetUnitColor(const GTBoard* b, int unit);

int GTBoard_RevealTile(GTBoard* b, int pos);

int GTBoard_CreateUnit(GTBoard* b, GTPlayer p, GTUnitType t, int pos);

int GTBoard_ResetUnitMovement(GTBoard* b, int unit);

int GTBoard_RemoveUnit(GTBoard* b, int unit);

int GTBoard_DamageUnit(GTBoard* b, int unit, int damage);

int GTBoard_MoveUnit(GTBoard* b, int unit, GTDirection d);

int GTBoard_ProduceUnit(GTBoard* b, int unit, GTUnitType t, GTDirection d);

int GTBoard_UndoPlay(GTBoard* b);

#endif
