#include "dbg.h"
#include "cmdline.h"
#include "lexer.h"
#include <string.h>

static const char* cmd[] =
{
  "",
  "mv",
  "rg",
  "pd",
  "st",
  "dn",
  "exit"
};

static const char* dir[] =
{
  "",
  "n",
  "e",
  "s",
  "w",
  "nn",
  "ee",
  "ss",
  "ww",
  "ne",
  "nw",
  "se",
  "sw"
};

static const char* unit[] =
{
  "",
  "g",
  "a",
  "c",
  "p",
  "f"
};

static const char whitespace[] = " \t\r\n";

int GTCommand_Init(GTCommand* c)
{
  c->rank = 0;
  c->file = 0;
  c->cmd = GTCommandType_None;
  c->d = GTDirection_None;
  c->t = GTUnitType_None;
  return 0;
}

int GTCommand_ParsePos(GTCommand* c, const char* tok)
{
  check(strlen(tok) == 2, "incorrect len");
  c->rank = tok[0] - 'a';
  c->file = tok[1] - '1';
  return 0;
  error:
  return -1;
}

int GTCommand_ParseCmd(GTCommand* c, const char* tok)
{
  int i;
  for (i = GTCommandType_Move; i <= GTCommandType_Stay; i++) {
    if (strcmp(cmd[i], tok) == 0) {
      c->cmd = i;
      return 0;
    }
  }
  return -1;
}

int GTCommand_ParseUnit(GTCommand* c, const char* tok)
{
  int i;
  for (i = GTUnitType_Gatherer; i < GTUnitType_Size; i++) {
    if (strcmp(unit[i], tok) == 0) {
      c->t = i;
      return 0;
    }
  }
  return -1;
}

int GTCommand_ParseDir(GTCommand* c, const char* tok)
{
  int i;
  for (i = GTDirection_North; i < GTDirection_Size; i++) {
    if (strcmp(dir[i], tok) == 0) {
      c->d = i;
      return 0;
    }
  }
  return -1;
}

int GTCommand_Parse(GTCommand* c, char* s)
{
  c->err = GTCommandError_None;
  GTLexer l;
  GTLexer_Init(&l, s);
  GTLexer_Skip(&l, whitespace);
  if (strcmp(cmd[GTCommandType_Exit], l.next) == 0) {
    c->cmd = GTCommandType_Exit;
    return 0;
  }
  if (strcmp(cmd[GTCommandType_Done], l.next) == 0) {
    c->cmd = GTCommandType_Done;
    return 0;
  }
  GTLexer_Skip(&l, whitespace);
  char* tok = GTLexer_GetToken(&l, whitespace);
  check(GTCommand_ParsePos(c, tok) == 0, "error parsing pos");
  GTLexer_Skip(&l, whitespace);
  tok = GTLexer_GetToken(&l, whitespace);
  check(GTCommand_ParseCmd(c, tok) == 0, "error parsing cmd");
  if (c->cmd == GTCommandType_Stay) { return 0; }
  if (c->cmd == GTCommandType_Produce) {
    GTLexer_Skip(&l, whitespace);
    tok = GTLexer_GetToken(&l, whitespace);
    check(GTCommand_ParseUnit(c, tok) == 0, "error parsing unit");
  }
  GTLexer_Skip(&l, whitespace);
  tok = GTLexer_GetToken(&l, whitespace);
  check(GTCommand_ParseDir(c, tok) == 0, "error parsing dir");
  return 0;
  error:
  c->err = GTCommandError_Parse;
  return -1;
}


