#ifndef _lexer_h_
#define _lexer_h_

#include "util.h"

struct GTLexer
{
  char* next;
};
struct_type(GTLexer);

int GTLexer_Init(GTLexer* l, char* s);
// get the token until a character in delimiters
char* GTLexer_GetToken(GTLexer* l, const char* delimiters);
// skips the characters in skip
int GTLexer_Skip(GTLexer* l, const char* skip);

#endif
