#ifndef __UART_H_
#define __UART_H_
#include "STC.h"
#include "Terminal.h"
char readByte();
void waitFor(char c);
void sendByte(char dat);
void sendStr(char *str);
void sendStr2(char *str,char *fcolor,char *bcolor);
void sendHexByte(unsigned char dat);
void sendHexString(char *str,int lenght);
void sendHexString2(char *str,unsigned int startaddr,unsigned int lenght);
void sendUInt(unsigned int number);
void sendLine(char *str);
void sendLine2(char *str,char *fcolor,char *bcolor);

void UartInit(void);

void outputError(char* notice,char* example);
#endif