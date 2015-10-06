#ifndef _GTAI_H_
#define _GTAI_H_

#include "types.h"
#include "cmdline.h"

#define enum_type(t) typedef enum t t
#define struct_type(t) typedef struct t t

struct GTBoard;

enum
{
  GTAI_UnitMax = 30,
  // movement + range + produce + stay
  GTAI_MoveMax = (4 * 2) + 12 + 4 + 1
};

enum GTAIError
{
  GTAIError_None,
  GTAIError_Size
};
enum_type(GTAIError);

struct GTAIMove
{
  int unit;
  GTCommandType cmd;
  GTDirection dir;
  GTUnitType type;
};
struct_type(GTAIMove);

int GTAIMove_ToCommand(GTAIMove* m, const struct GTBoard* b, GTCommand* c);

struct GTAIMoves
{
  int unitSize;
  // int unitId[GTAI_UnitMax];
  int moveSize[GTAI_UnitMax];
  GTAIMove moves[GTAI_UnitMax][GTAI_MoveMax];
};
struct_type(GTAIMoves);

int GTAIMoves_Init(GTAIMoves* m);
// generate legal moves for unit
int GTAIMoves_Generate(GTAIMoves* m, const struct GTBoard* b, int unit);
// generate legal moves for player p
int GTAIMoves_GenerateAll(GTAIMoves* m, const struct GTBoard* b, GTPlayer p);

struct GTAI
{
  GTPlayer p;
  struct GTBoard* b;
  GTAIMoves* m;
  GTAIError err;
};
struct_type(GTAI);

int GTAI_Init(GTAI* ai);
// plays random legal moves
int GTAI_Random(GTCommand* c);

int GTAI_PlayRandom(GTAI* ai, GTCommand* c);

#undef enum_type
#undef struct_type
#endif
