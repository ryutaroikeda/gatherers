#ifndef _GTIO_H_
#define _GTIO_H_

typedef char (*GTCharGetter)(void);

char GTGetCharFromStdin(void);
// read a line or up to size - 1 from getChar and terminate with '\0'
int GTGetLineExplicit(GTCharGetter getChar, char* buf, int size);
// read a line or up to size - 1 from stdin and terminate with '\0'
int GTGetLine(char* buf, int size);

#endif
