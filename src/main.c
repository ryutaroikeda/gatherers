#include "dbg.h"
#include "game.h"
#include "board.h"

int main(int argc, char* argv[])
{
  debug("%d %s", argc, argv[0]);
  GTBoard b;
  GTBoard_Init(&b);
  int pos1 = GTBoard_ValidMin + 2;
  int pos2 = GTBoard_InvalidMin - 3;
  GTBoard_CreateUnit(&b, GTPlayer_Black, GTUnitType_Gatherer, pos1);
  GTBoard_CreateUnit(&b, GTPlayer_White, GTUnitType_Gatherer, pos2);
  GTGame g;
  GTGame_Init(&g, &b);
  GTGame_Play(&g);
  return 0;
}
