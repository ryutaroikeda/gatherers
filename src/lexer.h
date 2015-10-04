#ifndef _GTLEXER_H_
#define _GTLEXER_H_

#define enum_type(t) typedef enum t t
#define struct_type(t) typedef struct t t

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

#undef enum_type
#undef struct_type
#endif
