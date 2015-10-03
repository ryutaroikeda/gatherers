#include "dbg.h"
#include "stream.h"
#include <unistd.h>

enum {
  GTStream_BufferSize = 1024
};

static char GTStream_GetString(GTStream *s)
{
  if (*s->stream.str) {
    return *s->stream.str++;
  } 
  s->err = GTStreamError_EndOfString;
  return '\0';
}

static char GTStream_GetFile(GTStream* s)
{
  char c = getc(s->stream.file);
  if (c == EOF) {
    s->err = GTStreamError_EndOfFile;
    return EOF;
  }
  return c;
}

static char GTStream_GetSocket(GTStream* s) 
{
  static char buffer[GTStream_BufferSize];
  static int next = 0;
  static int size = 0;
  if (next < size) {
    return buffer[next++];
  }
  next = 0;
  size = read(s->stream.fd, buffer, GTStream_BufferSize);
  if (size <= 0) { 
    s->err = GTStreamError_SocketFailed;
    return '\0';
  }
  return buffer[size++];
}

static int GTStream_Init(GTStream* s)
{
  s->err = GTStreamError_None;
  s->didSkip = 0;
  return 0;
}

int GTStream_InitString(GTStream* s, char* str)
{
  s->stream.str = str;
  // s->mode = GTStreamMode_String;
  s->get = &GTStream_GetString;
  return GTStream_Init(s);
}

int GTStream_InitFile(GTStream* s, FILE* file)
{
  s->stream.file = file;
  // s->mode = GTStreamMode_File;
  s->get = &GTStream_GetFile;
  return GTStream_Init(s);
}

int GTStream_InitSocket(GTStream* s, int socket)
{
  s->stream.fd = socket;
  // s->mode = GTStreamMode_RawFile;
  s->get = &GTStream_GetSocket;
  return GTStream_Init(s);
}

char GTStream_Get(GTStream* s)
{
  if (s->didSkip) {
    s->didSkip = 0;
    return s->skipped;
  }
  return (*s->get)(s);
}

int GTStream_Read(GTStream* s, char* buf, int size)
{
  int i;
  s->err = GTStreamError_None;
  for (i = 0; i < size; i++) {
    char c = GTStream_Get(s);
    if (s->err != GTStreamError_None) {
      buf[i] = '\0';
      return -1;
    }
    buf[i] = c;
  }
  return 0;
}

int GTStream_GetToken(GTStream* s, char* buf, int size, const char* delimiter)
{
  int i;
  s->err = GTStreamError_None;
  if (size <= 0) {
    return -1;
  }
  for (i = 0; i < size; i++) {
    char c = GTStream_Get(s);
    if (s->err != GTStreamError_None) {
      buf[i] = '\0';
      return -1;
    } else if (strchr(delimiter, c)) {
      buf[i] = '\0';
      return 0;
    }
    buf[i] = c;
  }
  buf[i - 1] = '\0';
  s->err = GTStreamError_TokenBufferLimitReached;
  return -1;
}

int GTStream_Skip(GTStream* s, const char* skip)
{
  s->err = GTStreamError_None;
  while (s->err == GTStreamError_None) {
    char c = GTStream_Get(s);
    if (!strchr(skip, c)) {
      s->skipped = c;
      s->didSkip = 1;
      return 0;
    }
  }
  // we reached the end of the stream
  return -1;
}
