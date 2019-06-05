#ifndef __UART_H_
#define __UART_H_
#include "STC.h"
#include "Terminal.h"
void SendByte(char dat);
void SendStr(char *str);
void SendStr2(char *str,char *fcolor,char *bcolor);
void SendHexByte(unsigned char dat);
void SendHexString(char *str,int lenght);
void SendHexString2(char *str,unsigned int startaddr,unsigned int lenght);
void SendUInt(unsigned int number);
void SendLine(char *str);
void SendLine2(char *str,char *fcolor,char *bcolor);

void UartInit(void);
#endif