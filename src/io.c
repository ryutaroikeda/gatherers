#include "dbg.h"
#include "io.h"
#include <stdio.h>

char GTGetCharFromStdin(void)
{
  return getc(stdin);
}

int GTGetLineExplicit(GTCharGetter getChar, char* buf, int size)
{
  int i = 0;
  if (size <= 0) {
    return -1;
  }
  while (i < size) {
    char c = (*getChar)();
    if (c == EOF || c == '\0') {
      buf[i] = '\0';
      return -1;
    } else if (c == '\n') {
      buf[i] = '\0';
      return 0;
    }
    buf[i] = c;
    i++;
  }
  buf[i - 1] = '\0';
  return -1;
}

int GTGetLine(char* buf, int size)
{
  return GTGetLineExplicit(&GTGetCharFromStdin, buf, size);
}
