#include "minunit.h"
#include "server.h"
#include "stream.h"
#include <string.h>

static char* Test_ParseRequest()
{
  char input[] = "GET / HTTP/1.1\r\nHost: localhost\r\n\r\n";
  GTConnection c;
  GTRequest req;
  GTResponse res;
  GTStream s;
  GTConnection_Init(&c, &req, &res);
  GTStream_InitString(&s, input);
  mu_assert(GTConnection_ParseRequest(&c, &s) == 0, "parse error");
  mu_assert(res.status == GTHttpStatus_Ok, ".status wrong");
  mu_assert(strcmp(req.url, "/") == 0, ".url wrong");
  return NULL;
}

static char* Test_All()
{
  mu_suite_start();
  mu_run_test(Test_ParseRequest);
  return NULL;
}

RUN_TESTS(Test_All)
