#include "dbg.h"
#include "netcmd.h"
#include "lexer.h"
#include <string.h>


static char* protocol[] =
{
  "", "start", "game"
};

int GTNetCommand_Init(GTNetCommand* nc)
{
  nc->type = GTNetCommandType_None;
  return 0;
}

int GTNetCommand_Parse(GTNetCommand* nc, char* s)
{
  GTLexer l;
  GTLexer_Init(&l, s);
  int i;
  char* tok;
  tok = GTLexer_GetToken(&l, ":");
  if (!tok) {
    log_err("missing protocol");
    return -1;
  }
  for (i = 0; i < GTNetCommandType_Size; i++) {
    if (strcmp(tok, protocol[i]) != 0) { break; }
    nc->type = i;
  }
  GTLexer_Skip(&l, " \r\t\n");
  tok = GTLexer_GetToken(&l, "\0");
  if (nc->type == GTNetCommandType_Game) {
    if (GTCommand_Parse(&nc->game, tok) == -1) {
      log_err("command parse failed");
      return -1;
    }
  }
  return 0;
}
