#ifndef _GTTYPES_H_
#define _GTTYPES_H_

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

enum GTUnitType
{
  GTUnitType_None,
  GTUnitType_Gatherer,
  GTUnitType_Archer,
  GTUnitType_Cavalry,
  GTUnitType_Spearman,
  GTUnitType_Fortress,
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

// GTBoard_Range() depends on this ordering
enum GTDirection
{
  GTDirection_None,
  GTDirection_North,
  GTDirection_East,
  GTDirection_South,
  GTDirection_West,
  GTDirection_NorthNorth,
  GTDirection_EastEast,
  GTDirection_SouthSouth,
  GTDirection_WestWest,
  GTDirection_NorthEast,
  GTDirection_NorthWest,
  GTDirection_SouthEast,
  GTDirection_SouthWest,
  GTDirection_Size
};
enum_type(GTDirection);

#undef enum_type
#undef struct_type
#endif
