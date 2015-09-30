#include "dbg.h"
#include "game.h"
#include "board.h"

int main(int argc, char* argv[])
{
  debug("%d %s", argc, argv[0]);
  char file[] =
   "tiles {"
  " w, m, h, i, w,"
  " p, h, w, m, h,"
  " m, i, p, m, i,"
  " i, m, p, i, m,"
  " h, m, w, h, p,"
  " w, i, h, m, w, }"
  "units {"
  "--,--,g1,--,--,"
  "--,--,--,--,--,"
  "--,--,--,--,--,"
  "--,--,--,--,--,"
  "--,--,--,--,--,"
  "--,--,G1,--,--, }";
  GTBoard b;
  GTBoard_Init(&b);
  GTBoard_Parse(&b, file);
  GTGame g;
  GTGame_Init(&g, &b);
  GTGame_Play(&g);
  return 0;
}
