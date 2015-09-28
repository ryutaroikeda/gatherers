#ifndef __cmdline_h__
#define __cmdline_h__

#include "util.h"
#include "types.h"

//
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
  GTCommandType_Done,
  GTCommandType_Exit,
  GTCommandType_Size
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
  GTCommandError err;
};
struct_type(GTCommand);

int GTCommand_Init(GTCommand* c);

int GTCommand_ParsePos(GTCommand* c, const char* tok);

int GTCommand_ParseCmd(GTCommand* c, const char* tok);

int GTCommand_ParseUnit(GTCommand* c, const char* tok);

int GTCommand_ParseDir(GTCommand* c, const char* tok);

int GTCommand_Parse(GTCommand* c, char* s);

#endif
