#include "dbg.h"

int main(int argc, char* argv[])
{
  debug("%d %s", argc, argv[0]);
  log_info("hello world");
  return 0;
// error:
  // return -1;
}
