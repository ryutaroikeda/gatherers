#ifndef _GTSTREAM_H_
#define _GTSTREAM_H_

#include <stdio.h>

#define enum_type(t) typedef enum t t
#define struct_type(t) typedef struct t t

struct GTStream;

typedef char (*GTSCharGetter)(struct GTStream*);

enum GTStreamMode
{
  GTStreamMode_None,
  GTStreamMode_ReadString,
  GTStreamMode_ReadFile,
  GTStreamMode_ReadRawFile,
  GTStreamMode_Size
};
enum_type(GTStreamMode);

enum GTStreamError
{
  GTStreamError_None,
  GTStreamError_EndOfString,
  GTStreamError_EndOfFile,
  GTStreamError_SocketFailed,
  GTStreamError_TokenBufferLimitReached,
  GTStreamError_Size
};
enum_type(GTStreamError);

struct GTStream
{
  union {
    char* str;
    FILE* file;
    int fd;
  } stream;
  char skipped;
  int didSkip;
  GTSCharGetter get;
  // GTStreamMode mode;
  GTStreamError err;
};
struct_type(GTStream);
// for internal use only
char GTStream_GetString(GTStream* s);
// for internal use only
char GTStream_GetFile(GTStream* s);
// for internal use only
char GTStream_GetSocket(GTStream* s);
// for internal use only
int GTStream_Init(GTStream* s);

int GTStream_InitString(GTStream* s, char* str);

int GTStream_InitFile(GTStream* s, FILE* file);

int GTStream_InitSocket(GTStream* s, int socket);

char GTStream_Get(GTStream* s);

int GTStream_GetToken(GTStream* s, char* buf, int size, char* delimiter);

int GTStream_Skip(GTStream* s, char* skip);

#undef enum_type
#undef struct_type

#endif
