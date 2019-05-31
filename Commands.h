#ifndef  __COMMANDS_H__
#define  __COMMANDS_H__
#include "STC15F2K60S2.h"
#include "Terminal.h"

typedef struct {
	const char *CommandName;
	void (*CommandFunc)();
	const char *Metadata;
	const char *HelpString;
}COMMAND;

extern char Argc;
extern char **Argv;

//基本操作
void Help();//帮助命令
void Prompt();//属性设置
void Clear();//清屏
void Reboot();//重启
//GPIO操作
void SetBit();//设置GPIO引脚
void ResetBit();//清零GPIO引脚
void GetBit();//获取GPIO引脚状态
// */

extern const COMMAND code CommandList[];


#endif