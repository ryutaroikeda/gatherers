#include "minunit.h"
#include "writer.h"
#include <string.h>

static char* Test_Write()
{
  char buf[100];
  GTWriter w;
  GTWriter_InitString(&w, buf, 100);
  GTWriter_Write(&w, "hello %d world", 594836);
  mu_assert(w.err == GTWriterError_None, "write failed");
  mu_assert(strcmp(buf, "hello 594836 world") == 0, "buf wrong");
  return NULL;
}

static char* Test_All()
{
  mu_suite_start();
  mu_run_test(Test_Write);
  return NULL;
}

RUN_TESTS(Test_All)
