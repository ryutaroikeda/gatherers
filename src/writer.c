#include "writer.h"

static int GTWriter_Init(GTWriter* w)
{
  w->err = GTWriterError_None;
  return 0;
}

int GTWriter_InitString(GTWriter* w, char* str, int size)
{
  w->stream.str.s = str;
  w->stream.str.cap = size;
  w->mode = GTWriterMode_String;
  GTWriter_Init(w);
  return 0;
}

int GTWriter_InitFile(GTWriter* w, FILE* file) 
{
  w->stream.file = file;
  w->mode = GTWriterMode_File;
  GTWriter_Init(w);
  return 0;
}
