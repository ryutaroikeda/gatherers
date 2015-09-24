#include "dbg.h"
#include "gatherers.h"

// static const GTUnitType tileProduct[GTTile_Size] =
// {
//   GTUnitType_None,
//   GTUnitType_None,
//   GTUnitType_Archer,
//   GTUnitType_Cavalry,
//   GTUnitType_Spearman,
//   GTUnitType_Castle,
//   GTUnitType_None
// };

static const uint8_t unitLife[GTUnitType_Size] =
{
  0, 1, 1, 2, 3, 4
};

// static const uint8_t unitCanMove[GTUnitType_Size] =
// {
//   0, 1, 1, 1, 1, 0
// };

static const uint8_t unitMovement[GTUnitType_Size] =
{
  0, 1, 1, 2, 1, 1
};

// static const uint8_t unitCanAttack[GTUnitType_Size] =
// {
//   0, 0, 0, 1, 1, 0
// };

// static const uint8_t unitRange[GTUnitType_Size] =
// {
//   0, 0, 2, 0, 0, 1
// };

int GTBoard_Init(GTBoard* b)
{
  debug("GT_Init");
  memset(b->tiles, GTTile_Plains, sizeof(GTTile) * GTBoard_Size);
  memset(b->board, -1, sizeof(uint8_t) * GTBoard_Size);
  b->unitId = 0;
  return 0;
// error:
  // return -1;
}

int GTBoard_CreateUnit(GTBoard* b, GTUnitType t, int pos)
{
  b->units[b->unitId] =
  (GTUnit){
    .type = t,
    .life = unitLife[t],
    .movement = unitMovement[t]
  };
  b->board[pos] = b->unitId;
  b->unitId++;
  return 0;
// error:
  // return -1;
}
