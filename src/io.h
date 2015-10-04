#ifndef _GTIO_H_
#define _GTIO_H_

#define enum_type(t) typedef enum t t
#define struct_type(t) typedef struct t t

typedef char (*GTCharGetter)(void);

char GTGetCharFromStdin(void);
// read a line or up to size - 1 from getChar and terminate with '\0'
int GTGetLineExplicit(GTCharGetter getChar, char* buf, int size);
// read a line or up to size - 1 from stdin and terminate with '\0'
int GTGetLine(char* buf, int size);

#undef enum_type
#undef struct_type
#endif
