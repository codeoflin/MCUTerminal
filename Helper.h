#ifndef  __Helper_H__
#define  __Helper_H__

#include <string.h>
void toLower(const char* str);
long int toLong(unsigned char *line);
int index(char *str,char c);
int ParseHEX(char * hex,char * buff,int maxlenght);
#endif