#ifndef _GTWRITER_H_
#define _GTWRITER_H_

#include <stdio.h>

#define enum_type(t) typedef enum t t
#define struct_type(t) typedef struct t t

enum GTWriterMode
{
  GTWriterMode_None,
  GTWriterMode_String,
  GTWriterMode_File,
  // GTWriterMode_Socket,
  GTWriterMode_Size
};
enum_type(GTWriterMode);

enum GTWriterError
{
  GTWriterError_None,
  GTWriterError_StringBufferLimitReached,
  GTWriterError_FileError,
  // GTWriterError_SocketError,
  GTWriterError_Size
};
enum_type(GTWriterError);

struct GTWriter
{
  union {
    struct {
      char* s;
      int cap;
    } str;
    FILE* file;
    // int fd;
  } stream;
  GTWriterMode mode;
  GTWriterError err;
};
struct_type(GTWriter);

int GTWriter_InitString(GTWriter* w, char* s, int size);

int GTWriter_InitFile(GTWriter* w, FILE* file);

#define GTWriter_Min(x, y) ((x) < (y) ? (x) : (y))

#define GTWriter_Write(w, ...)                                               \
do {                                                                         \
  if ((w)->mode == GTWriterMode_String) {                                    \
    int len = snprintf((w)->stream.str.s, (w)->stream.str.cap, __VA_ARGS__); \
    int n = GTWriter_Min(len, (w)->stream.str.cap);                          \
    (w)->stream.str.s += n;                                                  \
    (w)->stream.str.cap -= n;                                                \
    if ((w)->stream.str.cap <= 0) {                                          \
      (w)->err = GTWriterError_StringBufferLimitReached;                     \
    }                                                                        \
  } else if ((w)->mode == GTWriterMode_File) {                               \
    if (fprintf((w)->stream.file, __VA_ARGS__) < 0) {                        \
      (w)->err = GTWriterError_FileError;                                    \
    }                                                                        \
  }                                                                          \
} while (0)   

#endif
