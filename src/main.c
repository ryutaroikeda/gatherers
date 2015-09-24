#include "dbg.h"

int main(int argc, char* argv[])
{
  debug("%d %s", argc, argv[0]);
  return 0;
// error:
  // return -1;
}
