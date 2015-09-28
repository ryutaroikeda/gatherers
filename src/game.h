#ifndef __game_h__
#define __game_h__

#include "util.h"
#include "cmdline.h"
#include "board.h"

enum
{
  GTGame_BlackStart = GTBoard_ValidMin + (GTBoard_Width / 2),
  GTGame_WhiteStart = GTBoard_InvalidMin - 1 - (GTBoard_Width / 2)
};

struct GTGame
{
  GTPlayer p;
  struct GTBoard b;
};

struct_type(GTGame);

int GTGame_Init(GTGame* g);
// return 1 if the game has ended
int GTGame_IsEnd(const GTGame* g);
// return -1 on exit command
int GTGame_DoCommand(GTGame* g, GTCommand* c);

int GTGame_EndTurn(GTGame* g);

int GTGame_Play(GTGame* g);

#endif
