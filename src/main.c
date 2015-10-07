#include "dbg.h"
#include "game.h"
#include "board.h"
#include "ai.h"
#include "server.h"
#include <stdlib.h>
#include <string.h>

int printhelp(void)
{
  return fprintf(stdout,
      "Usage: \n"
      "gatherers cmdline\n"
      "gatherers server <port>\n"
      );
} 

int main(int argc, char* argv[])
{
  if (argc < 1) {
    printhelp();
    return -1;
  }
  if (strcmp(argv[1], "cmdline") == 0) {
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
    GTAI_TryScorePlayer(&c);
    c.cmd = GTCommandType_SetPlayer;
    c.rank = (int) GTPlayer_Black;
    GTAI_TryScorePlayer(&c);
    g.interface[GTPlayer_Black] = &GTAI_TryScorePlayer;
    g.interface[GTPlayer_White] = &GTCommand_GetStdin;
    GTGame_Play(&g);
  } else if (strcmp(argv[1], "server") == 0) {
    if (argc < 2) {
      printhelp();
      return -1;
    }
    long port = strtol(argv[2], NULL, 10);
    fprintf(stdout, "starting server on port %ld\n", port);
    GTServer s;
    GTServer_Init(&s, port);
    s.listenSize = 10;
    GTServer_Run(&s);
  } else {
    printhelp();
    return -1;
  }
  return 0;
}
