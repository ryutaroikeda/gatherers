#include "dbg.h"
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

static const GTUnitType tileProduct[GTTile_Size] =
{
  GTUnitType_None,
  GTUnitType_None,
  GTUnitType_Archer,
  GTUnitType_Cavalry,
  GTUnitType_Spearman,
  GTUnitType_Castle,
  GTUnitType_None
};

static const uint8_t unitLife[GTUnitType_Size] =
{
  0, 1, 1, 2, 3, 4
};

static const uint8_t unitCanMove[GTUnitType_Size] =
{
  0, 1, 1, 1, 1, 0
};

static const uint8_t unitMovement[GTUnitType_Size] =
{
  0, 1, 1, 2, 1, 1
};

static const uint8_t unitCanAttack[GTUnitType_Size] =
{
  0, 0, 0, 1, 1, 0
};

static const uint8_t unitRange[GTUnitType_Size] =
{
  0, 0, 2, 0, 0, 1
};

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

static GTUnit units[GTBoard_UnitSize];

// store the tile of each position
static GTTile tiles[GTBoard_Size];

// store the index of the unit on each position
static uint8_t board[GTBoard_Size];

static int unitId;

int GT_Init()
{
  debug("GT_Init");
  memset(tiles, GTTile_Plains, sizeof(GTTile) * GTBoard_Size);
  memset(board, -1, sizeof(uint8_t) * GTBoard_Size);
  unitId = 0;
  return 0;
error:
  return -1;
}

int GT_CreateUnit(GTUnitType t, int pos)
{
  units[unitId] =
  (GTUnit){
    .type = t,
    .life = unitLife[t],
    .movement = unitMovement[t]
  };
  board[pos] = unitId;
  unitId++;
  return 0;
error:
  return -1;
}
