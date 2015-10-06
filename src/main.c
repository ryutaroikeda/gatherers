#include "dbg.h"
// #include "game.h"
// #include "board.h"
// #include "ai.h"
#include "server.h"
#include <stdlib.h>

int main(int argc, char* argv[])
{
  if (argc < 2) {
    fprintf(stdout, "Usage: gatherers <port>\n");
    return -1;
  }
  long port = strtol(argv[1], NULL, 10);
  GTServer s;
  GTServer_Init(&s, port);
  s.listenSize = 10;
  GTServer_Run(&s);

  // char file[] =
  //  "tiles {"
  // " w, m, h, i, w,"
  // " p, h, w, m, h,"
  // " m, i, p, m, i,"
  // " i, m, p, i, m,"
  // " h, m, w, h, p,"
  // " w, i, h, m, w, }"
  // "units {"
  // "--,--,g1,--,--,"
  // "--,--,--,--,--,"
  // "--,--,--,--,--,"
  // "--,--,--,--,--,"
  // "--,--,--,--,--,"
  // "--,--,G1,--,--, }";
  // GTBoard b;
  // GTBoard_Init(&b);
  // GTBoard_Parse(&b, file);
  // GTGame g;
  // GTGame_Init(&g, &b);
  // GTCommand c;
  // c.cmd = GTCommandType_SetBoard;
  // c.b = &b;
  // check(GTAI_Random(&c) == 2, "board set failed");
  // c.cmd = GTCommandType_SetPlayer;
  // c.rank = (int) GTPlayer_Black;
  // check(GTAI_Random(&c) == 1, "player set failed");
  // g.interface[GTPlayer_Black] = &GTAI_Random;
  // g.interface[GTPlayer_White] = &GTCommand_GetStdin;
  // GTGame_Play(&g);

  return 0;
  // error:
  // return -1;
}
