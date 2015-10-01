#ifndef _GTGAME_H_
#define _GTGAME_H_

#include "util.h"
#include "types.h"

struct GTBoard;
struct GTCommand;

typedef int (*GTCommandGetter)(struct GTCommand*);

struct GTGame
{
  // the current player
  GTPlayer p;
  // functions to call for getting command input
  GTCommandGetter interface[GTPlayer_Size];
  // the board state
  struct GTBoard* b;
};
struct_type(GTGame);

int GTGame_Init(GTGame* g, struct GTBoard* b);
// return 1 if the game has ended
int GTGame_IsEnd(const GTGame* g);
// return -1 on error or exit command
int GTGame_DoCommand(GTGame* g, struct GTCommand* c);
// get command from interface
int GTGame_GetCommand(GTGame* g, struct GTCommand* c);

int GTGame_EndTurn(GTGame* g);

int GTGame_Play(GTGame* g);

int GTGame_PlayStdin(GTGame* g);

int GTGame_PrintInfo(const GTGame* g, FILE* stream);

#endif
