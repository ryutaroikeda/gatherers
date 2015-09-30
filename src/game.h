#ifndef _GTGAME_H_
#define _GTGAME_H_

#include "util.h"
#include "types.h"

struct GTBoard;
struct GTCommand;

struct GTGame
{
  GTPlayer p;
  struct GTBoard* b;
};
struct_type(GTGame);

typedef int (*CommandGetter)(struct GTCommand*);

int GTGame_Init(GTGame* g, struct GTBoard* b);
// return 1 if the game has ended
int GTGame_IsEnd(const GTGame* g);
// return -1 on exit command
int GTGame_DoCommand(GTGame* g, struct GTCommand* c);

int GTGame_EndTurn(GTGame* g);

int GTGame_PlayExplicit(GTGame* g, CommandGetter cg);

int GTGame_Play(GTGame* g);

int GTGame_PrintInfo(const GTGame* g, FILE* stream);

#endif
