#include "minunit.h"
#include "server.h"
#include "stream.h"
#include <string.h>

static char* Test_ParseRequest()
{
  char* input = "GET /gatherers.com HTTP/1.1\r\nHost: localhost\r\n\r\n";
  GTConnection c;
  GTRequest req;
  GTResponse res;
  GTStream s;
  GTRequest_Init(&req);
  GTResponse_Init(&res);
  GTConnection_Init(&c, &req, &res);
  GTStream_InitString(&s, input);
  mu_assert(GTConnection_ParseRequest(&c, &s) == 0, "parse error");
  mu_assert(res.status == GTHttpStatus_Ok, ".status wrong");
  mu_assert(strcmp(req.url, "gatherers.com") == 0, ".url wrong");
  mu_assert(req.contentLength == 0, ".contentLength wrong");
  input =
  "GET / HTTP/1.1\r\n"
  "Host: localhost\r\n"
  "Accept: text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8\r\n"
  "Accept-Language: en-us\r\n"
  "Connection: keep-alive\r\n"
  "Accept-Encoding: gzip, deflate\r\n"
  "User-Agent: Mozilla/5.0 (Macintosh; Intel Mac OS X 10_9_4) "
  "AppleWebKit/537.78.2 (KHTML, like Gecko) Version/7.0.6 Safari/537.78.2\r\n"
  "\r\n";
  GTRequest_Init(&req);
  GTResponse_Init(&res);
  GTConnection_Init(&c, &req, &res);
  GTStream_InitString(&s, input);
  mu_assert(GTConnection_ParseRequest(&c, &s) == 0, "parse error");
  mu_assert(strcmp(req.host, "localhost") == 0, ".host wrong");
  return NULL;
}

static char* Test_All()
{
  mu_suite_start();
  mu_run_test(Test_ParseRequest);
  return NULL;
}

RUN_TESTS(Test_All)
