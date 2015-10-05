#ifndef _GTNETCOMMAND_H_
#define _GTNETCOMMAND_H_

#include "cmdline.h"

#define enum_type(t) typedef enum t t
#define struct_type(t) typedef struct t t

// enum {
  // GTNetCommand_CommandSize 
// };

enum GTNetCommandType {
  GTNetCommandType_None,
  GTNetCommandType_Start,
  GTNetCommandType_Game,
  GTNetCommandType_Size
};
enum_type(GTNetCommandType);

struct GTNetCommand {
  GTNetCommandType type;
  GTCommand game;
};
struct_type(GTNetCommand);

int GTNetCommand_Init(GTNetCommand* nc);

int GTNetCommand_Parse(GTNetCommand* nc, char* s);

#undef enum_type
#undef struct_type

#endif
