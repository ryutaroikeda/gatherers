#ifndef __gatherers_h__
#define __gatherers_h__

#include <stdint.h>

#define enum_type(t) typedef enum t t
#define struct_type(t) typedef struct t t

enum GTTile
{
  GTTile_None,
  GTTile_Plains,
  GTTile_Wood,
  GTTile_Horse,
  GTTile_Iron,
  GTTile_Lake,
  GTTile_Mountain,
  GTTile_Size
};

enum_type(GTTile);

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
  GTUnitType type;
  uint8_t life;
  uint8_t movement;
};

struct_type(GTUnit);

enum
{
  GTBoard_Width = 5,
  GTBoard_Height = 6,
  GTBoard_WidthMax = 1 + GTBoard_Width + 1,
  GTBoard_HeightMax = 1 + GTBoard_Height + 1,
  GTBoard_Size = GTBoard_WidthMax * GTBoard_HeightMax,
  GTBoard_ValidMin = GTBoard_WidthMax + 1,
  GTBoard_InvalidMin = GTBoard_Size - GTBoard_WidthMax - 1,

  // do not recycle units
  // this is to speed up the ai
  GTBoard_UnitSize = 1000
};

struct GTBoard
{
  GTUnit units[GTBoard_UnitSize];
  // store the tile of each position
  GTTile tiles[GTBoard_Size];
  // store the index of the unit on each position
  uint8_t board[GTBoard_Size];
  int unitId;
};

struct_type(GTBoard);

int GTBoard_Init(GTBoard* b);

int GTBoard_CreateUnit(GTBoard* b, GTUnitType t, int pos);



#endif
