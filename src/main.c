#include "dbg.h"
#include "game.h"
#include "board.h"
#include "ai.h"
#include "server.h"
#include <stdlib.h>

int main(int argc, char* argv[])
{
  if (argc < 2) {
    fprintf(stdout, "Usage: gatherers <port>\n");
    return -1;
  }
  (void) argv;
  // long port = strtol(argv[1], NULL, 10);
  // GTServer s;
  // GTServer_Init(&s, port);
  // s.listenSize = 10;
  // GTServer_Run(&s);
  char file[] =
  "units {"
  "--,--,G1,--,--,"
  "--,--,--,--,--,"
  "--,--,--,--,--,"
  "--,--,--,--,--,"
  "--,--,--,--,--,"
  "--,--,g1,--,--,}";
  int f[] = { 0, 400, 600, 600, 550, 250, 600 };
  GTBoardConfig conf;
  memcpy(conf.frequency, f, sizeof(int) * 7);
  GTBoard b;
  GTBoard_Init(&b);
  GTBoard_Generate(&b, &conf);
  GTBoard_Parse(&b, file);
  GTGame g;
  GTGame_Init(&g, &b);
  GTCommand c;
  c.cmd = GTCommandType_SetBoard;
  c.b = &b;
  check(GTAI_TryScorePlayer(&c) == 2, "board set failed");
  c.cmd = GTCommandType_SetPlayer;
  c.rank = (int) GTPlayer_Black;
  check(GTAI_TryScorePlayer(&c) == 1, "player set failed");
  g.interface[GTPlayer_Black] = &GTAI_TryScorePlayer;
  g.interface[GTPlayer_White] = &GTCommand_GetStdin;
  GTGame_Play(&g);

  return 0;
  error:
  return -1;
}
