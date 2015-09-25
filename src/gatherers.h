#ifndef __gatherers_h__
#define __gatherers_h__

#include <stddef.h>

#define enum_type(t) typedef enum t t
#define struct_type(t) typedef struct t t

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
  int isVisible;
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

struct GTStackEntry
{
  int val;
  int* addr;
};
struct_type(GTStackEntry);

struct GTStack
{
  // maximum stack depth
  int size;
  // index of the top of the stack
  int ptr;
  // entry of the top of the stack
  GTStackEntry* top;
};
struct_type(GTStack);

enum GTStackCode
{
  GTStackCode_None,
  GTStackCode_BeginPlay,
  GTStackCode_BeginTurn,
  GTStackCode_Size
};
enum_type(GTStackCode);

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

int GTStack_Init(GTStack* s, GTStackEntry* entries, size_t size);

int GTStack_PushExplicit(GTStack* s, int val, int* addr);

#define GTStack_Push(s, v) GTStack_PushExplicit(s, (v), (int*)&(v))

// #define GTStack_PushAndSet(s, u, v) do \
// { GTStack_Push(s, (u)); \
//   u = (v); \
// } while(0)

int GTStack_Pop(GTStack* s);

int GTStack_Peek(GTStack* s, GTStackEntry* e);

int GTStack_BeginPlay(GTStack* s);

int GTStack_BeginTurn(GTStack* s);

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
  // index of next free unit
  int unitId;
};
struct_type(GTBoard);

int GTBoard_Init(GTBoard* b);
// return 1 if .board[pos] is valid
int GTBoard_IsValid(GTBoard* b, int pos);
// return 1 if .board[pos] is empty
int GTBoard_IsEmpty(GTBoard* b, int pos);
// return 1 if .board[pos] is a unit
int GTBoard_IsUnit(GTBoard* b, int pos);
// return 1 if .tiles[pos] is visible
int GTBoard_IsVisible(GTBoard* b, int pos);

int GTBoard_RevealTile(GTBoard* b, int pos);

int GTBoard_CreateUnit(GTBoard* b, GTPlayer p, GTUnitType t, int pos);

#endif
