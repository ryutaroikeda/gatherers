#include "minunit.h"
#include "lexer.h"

static char* Test_GTLexer_GetToken()
{
  GTLexer l;
  char s[] = "hello, world";
  GTLexer_Init(&l, s);
  char* tok = GTLexer_GetToken(&l, ",");
  mu_assert(strcmp(tok, "hello") == 0, "tok wrong");
  tok = GTLexer_GetToken(&l, ",");
  mu_assert(strcmp(tok, " world") == 0, "tok wrong");
  mu_assert(strlen(l.next) == 0, "strlen wrong");
  return NULL;
}

static char* Test_GTLexer_Skip()
{
  GTLexer l;
  char s[] = " \n \r\t  \t world";
  GTLexer_Init(&l, s);
  mu_assert(GTLexer_Skip(&l, " \n\r\t") == 0, "skip failed");
  char* tok = GTLexer_GetToken(&l, " ");
  mu_assert(strcmp(tok, "world") == 0, "tok wrong");
  return NULL;
}

static char* Test_All()
{
  mu_suite_start();
  mu_run_test(Test_GTLexer_GetToken);
  mu_run_test(Test_GTLexer_Skip);
  return NULL;
}

RUN_TESTS(Test_All)
