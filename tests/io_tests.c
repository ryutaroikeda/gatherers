#include "minunit.h"
#include "io.h"

static char input[] = "hello world\nfarewell world\nthe end";

static char getChar(void) {
  static char* p = input;
  if (*p == '\0') { return '\0'; }
  return *(p++);
}

static char* Test_GTGetLineExplicit()
{
  char buf[256];
  mu_assert(GTGetLineExplicit(&getChar, buf, 256) == 0, "GetLine error");
  mu_assert(strcmp(buf, "hello world") == 0, "got wrong line");
  mu_assert(GTGetLineExplicit(&getChar, buf, 256) == 0, "getline error");
  mu_assert(strcmp(buf, "farewell world") == 0, "got wrong line");
  mu_assert(GTGetLineExplicit(&getChar, buf, 256) == -1, "didn't report error");
  mu_assert(strcmp(buf, "the end") == 0, "got wrong line");
  return NULL;
}

static char* Test_All()
{
  mu_suite_start();
  mu_run_test(Test_GTGetLineExplicit);
  return NULL;
}

RUN_TESTS(Test_All)
