#include "minunit.h"
#include "stream.h"
#include <string.h>

static char* Test_GetString()
{
  char buf[] = "war";
  GTStream s;
  GTStream_InitString(&s, buf);
  mu_assert(GTStream_GetString(&s) == 'w', "wrong char");
  mu_assert(GTStream_GetString(&s) == 'a', "wrong char");
  mu_assert(GTStream_GetString(&s) == 'r', "wrong char");
  mu_assert(s.err == GTStreamError_None, ".err wrong");
  mu_assert(GTStream_GetString(&s) == '\0', "wrong char");
  mu_assert(s.err == GTStreamError_EndOfString, ".err wrong");
  return NULL;
}

static char* Test_GetToken()
{
  char buf[] = "hello world";
  GTStream s;
  GTStream_InitString(&s, buf);
  char tok[100];
  mu_assert(GTStream_GetToken(&s, tok, 100, " ") == 0, "get token failed");
  mu_assert(strcmp(tok, "hello") == 0, "tok wrong");
  memset(tok, 0, 100);
  mu_assert(GTStream_GetToken(&s, tok, 100, " ") == -1,
   "undetected end of string");
  mu_assert(strcmp(tok, "world") == 0, "tok wrong");
  return NULL;
}

static char* Test_Skip()
{
  char buf[] = "hello       world      ";
  GTStream s;
  GTStream_InitString(&s, buf);
  char tok[100];
  GTStream_GetToken(&s, tok, 100, " ");
  mu_assert(GTStream_Skip(&s, " ") == 0, "skip error");
  memset(tok, 0, 100);
  mu_assert(GTStream_GetToken(&s, tok, 100, " ") == 0, "get token failed");
  mu_assert(strcmp(tok, "world") == 0, "tok wrong");
  mu_assert(GTStream_Skip(&s, " ") == -1, "undetected end of string");
  return NULL;
}

static char* Test_All()
{
  mu_suite_start();
  mu_run_test(Test_GetString);
  mu_run_test(Test_GetToken);
  mu_run_test(Test_Skip);
  return NULL;
}

RUN_TESTS(Test_All)
