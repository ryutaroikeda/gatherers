#ifndef _GTCMDLINE_H_
#define _GTCMDLINE_H_

#include "types.h"
#include "io.h"

#define enum_type(t) typedef enum t t
#define struct_type(t) typedef struct t t

// syntax of cmdmands
//
// Command = Move | Range | Produce | Stay | Done | Exit
// Move = Pos mv Dir
// Range = Pos rg Dir
// Produce = Pos pd Unit Dir
// Stay = Pos st
// Done = dn
// Exit = exit
// Pos = [a...e][1...6]
// Dir = n | e | s | w | nn | ee | ss | ww | ne | nw | se | sw
// Unit = g | a | c | p | f
//

enum GTCommandType
{
  GTCommandType_None,
  GTCommandType_Move,
  GTCommandType_Range,
  GTCommandType_Produce,
  GTCommandType_Stay,
  GTCommandType_Undo,
  GTCommandType_Done,
  GTCommandType_Exit,
  GTCommandType_Info,
  GTCommandType_Size,
  // special commands for interfaces
  GTCommandType_SetPlayer,
  GTCommandType_SetBoard
};
enum_type(GTCommandType);

enum GTCommandError
{
  GTCommandError_None,
  GTCommandError_Parse,
  GTCommandError_Illegal,
  GTCommandError_Exit,
  GTCommandError_Size
};
enum_type(GTCommandError);

struct GTCommand
{
  int rank;
  int file;
  GTCommandType cmd;
  GTDirection d;
  GTUnitType t;
  struct GTBoard* b;
  GTCommandError err;
};
struct_type(GTCommand);

int GTCommand_Init(GTCommand* c);

int GTCommand_ParsePos(GTCommand* c, const char* tok);

int GTCommand_ParseCmd(GTCommand* c, const char* tok);

int GTCommand_ParseUnit(GTCommand* c, const char* tok);

int GTCommand_ParseDir(GTCommand* c, const char* tok);

int GTCommand_Parse(GTCommand* c, char* s);
// get command from a stream
int GTCommand_Get(GTCommand* c, GTCharGetter g);
// get command from stdin
int GTCommand_GetStdin(GTCommand* c);

#undef enum_type
#undef struct_type
#endif
