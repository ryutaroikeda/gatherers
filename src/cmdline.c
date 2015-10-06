#include "dbg.h"
#include "cmdline.h"
#include "lexer.h"
#include "writer.h"
#include <string.h>

static const char* cmd[GTCommandType_Size] =
{
  "",
  "mv",
  "rg",
  "pd",
  "st",
  "undo",
  "done",
  "exit",
  "info"
};

static const char* dir[GTDirection_Size] =
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

static const char* unit[GTUnitType_Size] =
{
  "",
  "g",
  "a",
  "c",
  "s",
  "f"
};

static const char whitespace[] = " \t\r\n";

static const int isMetaCmd[GTCommandType_Size] =
{
  0, 0, 0, 0, 0, 1, 1, 1, 1
};

int GTCommand_Init(GTCommand* c)
{
  c->rank = 0;
  c->file = 0;
  c->cmd = GTCommandType_None;
  c->d = GTDirection_None;
  c->t = GTUnitType_None;
  c->b = NULL;
  c->err = GTCommandError_None;
  return 0;
}

int GTCommand_ParsePos(GTCommand* c, const char* tok)
{
  check(strlen(tok) == 2, "incorrect len");
  c->rank = tok[0] - 'a';
  c->file = tok[1] - '1';
  log_info("parsepos: rank = %d, file = %d\n", c->rank, c->file);
  return 0;
  error:
  return -1;
}

int GTCommand_ParseCmd(GTCommand* c, const char* tok)
{
  int i;
  for (i = GTCommandType_None; i < GTCommandType_Size; i++) {
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
  char* tok;
  GTLexer l;
  GTLexer_Init(&l, s);
  GTLexer_Skip(&l, whitespace);
  tok = GTLexer_GetToken(&l, whitespace);
  check(GTCommand_ParseCmd(c, tok) == 0, "error parsing command");
  if (isMetaCmd[c->cmd]) { return 0; }
  GTLexer_Skip(&l, whitespace);
  tok = GTLexer_GetToken(&l, whitespace);
  check(GTCommand_ParsePos(c, tok) == 0, "error parsing pos");
  if (c->cmd == GTCommandType_Stay) { return 0; }
  GTLexer_Skip(&l, whitespace);
  tok = GTLexer_GetToken(&l, whitespace);
  check(GTCommand_ParseDir(c, tok) == 0, "error parsing dir");
  if (c->cmd == GTCommandType_Produce) {
    GTLexer_Skip(&l, whitespace);
    tok = GTLexer_GetToken(&l, whitespace);
    check(GTCommand_ParseUnit(c, tok) == 0, "error parsing unit");
  }
  return 0;
  error:
  c->err = GTCommandError_Parse;
  return -1;
}

int GTCommand_Get(GTCommand* c, GTCharGetter g)
{
  GTCommand_Init(c);
  char buf[256];
  if (GTGetLineExplicit(g, buf, 256) == -1) {
    log_warn("something wrong with GetLine");
  }
  check(GTCommand_Parse(c, buf) == 0, "Parse failed");
  return 0;
  error:
  return -1;
}

int GTCommand_GetStdin(GTCommand* c)
{
  return GTCommand_Get(c, &GTGetCharFromStdin);
}

int GTCommand_Write(GTCommand* c, GTWriter* w)
{
  GTWriter_Write(w, "%s", cmd[c->cmd]);
  if (isMetaCmd[c->cmd]) {
    GTWriter_Write(w, "\n");
    return 0;
  }
  GTWriter_Write(w, " %c%c", c->rank + 'a', c->file + '1');
  if (c->cmd == GTCommandType_Stay) { 
    GTWriter_Write(w, "\n");
    return 0;
  }
  GTWriter_Write(w, " %s", dir[c->d]);
  if (c->cmd != GTCommandType_Produce) {
    GTWriter_Write(w, "\n");
    return 0;
  }
  GTWriter_Write(w, " %s\n", unit[c->t]);
  return 0;
}
