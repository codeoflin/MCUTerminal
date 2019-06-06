#ifndef  __Helper_H__
#define  __Helper_H__

#include <string.h>
void toLower(const char* str);
long int toLong(unsigned char *line);
int index(char *str,char c);
int parseHex(char * hex,char * buff,int maxlenght);
unsigned int crc16(unsigned char *ptr,unsigned int count);
unsigned int checksum16(unsigned char *ptr,unsigned int count);
unsigned char checksum8(unsigned char *pBuffer, unsigned int size);
#endif