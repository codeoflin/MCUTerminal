#ifndef  __COMMANDS_H__
#define  __COMMANDS_H__
#include "STC15F2K60S2.h"
#include "Terminal.h"
#include <string.h>

#define WT_30M  0x80
void writeFlash(unsigned int addr,unsigned char dat);
unsigned char readFlash(unsigned int addr);

typedef struct {
	const char *CommandName;
	void (*CommandFunc)();
	const char *Metadata;
	const char *HelpString;
}COMMAND;

extern char Argc;
extern char **Argv;

//基本操作
void help();//帮助命令
void clear();//清屏
void reboot();//重启
//GPIO操作
void setBit();//设置GPIO引脚
void resetBit();//清零GPIO引脚
void getBit();//获取GPIO引脚状态
//flash操作
void flash();
// */

extern const COMMAND code CommandList[];
#endif