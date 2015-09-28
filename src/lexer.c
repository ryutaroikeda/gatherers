#include "dbg.h"
#include "lexer.h"
#include <string.h>

int GTLexer_Init(GTLexer* l, char* s)
{
  l->next = s;
  return 0;
}

char* GTLexer_GetToken(GTLexer* l, const char* delimiters)
{
  char* tok = l->next;
  char c;
  while ((c = *l->next)) {
    l->next++;
    if (strchr(delimiters, c)) {
      *(l->next - 1) = '\0';
      break;
    }
  }
  return tok;
}

int GTLexer_Skip(GTLexer* l, const char* skip)
{
  while(strchr(skip, *l->next)) {
    if (!*l->next) { break; }
    l->next++;
  }
  return 0;
}
