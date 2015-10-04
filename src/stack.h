#ifndef _GTSTACK_H_
#define _GTSTACK_H_

#define enum_type(t) typedef enum t t
#define struct_type(t) typedef struct t t

enum GTStackCode
{
  GTStackCode_None,
  GTStackCode_BeginPlay,
  GTStackCode_BeginTurn,
  GTStackCode_Size
};
enum_type(GTStackCode);

enum GTStackError
{
  GTStackError_None,
  GTStackError_Underflow,
  GTStackError_Overflow,
  GTStackError_EmptyPurge,
  GTStackError_EmptyPeek,
  GTStackError_Size
};
enum_type(GTStackError);

// keep the address and the old value
struct GTStackEntry
{
  int val;
  int* addr;
};
struct_type(GTStackEntry);

struct GTStack
{
  // entry of the top of the stack
  GTStackEntry* top;
  // maximum stack depth
  int size;
  // index of the top of the stack
  int ptr;
  // error number
  GTStackError err;
};
struct_type(GTStack);

int GTStack_Init(GTStack* s, GTStackEntry* entries, int size);
// return 1 if stack is empty
int GTStack_IsEmpty(const GTStack* s);

int GTStack_PushExplicit(GTStack* s, int val, int* addr);

#define GTStack_Push(s, v) GTStack_PushExplicit(s, (v), (int*)&(v))

int GTStack_PushAndSetExplicit(GTStack* s, int val, int* addr, int set);

#define GTStack_PushAndSet(s, v, w) \
GTStack_PushAndSetExplicit(s, (v), (int*)&(v), w)

// remove the top and restore
int GTStack_Pop(GTStack* s);
// remove the top without restoring
int GTStack_Purge(GTStack* s);

int GTStack_Peek(GTStack* s, GTStackEntry* e);

int GTStack_BeginPlay(GTStack* s);

int GTStack_BeginTurn(GTStack* s);

#undef enum_type
#undef struct_type
#endif
