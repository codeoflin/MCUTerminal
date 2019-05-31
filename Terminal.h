#ifndef  __TERMINAL_H__
#define  __TERMINAL_H__
#include "STC15F2K60S2.h"
#include "Commands.h"
#include "Uart.h"
#include "Helper.h"
#include <string.h>
#include <intrins.h>
#include <stdio.h>
#include <ctype.h>
#define MAX_SERIAL_BUFFER_SIZE		64
#define MAX_PROMPT_BUFFER_SIZE		8
#define MAX_VT_COMMAND_BUFFER_SIZE	16
extern char idata SerialBuffer[MAX_SERIAL_BUFFER_SIZE];
extern char idata PromptBuffer[MAX_PROMPT_BUFFER_SIZE];

typedef enum {
	PS_WHITESPACE,
	PS_TOKEN,
	PS_STRING,
	PS_ESCAPE
}PARSESTATE;

/*前景色*/
#define    F_BLACK 			"\033[30m" 			/* 黑色 */
#define    F_RED 			"\033[31m" 			/* 红色 */
#define    F_GREEN 			"\033[32m" 			/* 绿色 */
#define    F_YELLOW 		"\033[33m" 			/* 黄色 */
#define    F_BLUE 			"\033[34m" 			/* 蓝色 */
#define    F_MAGENTA 		"\033[35m" 			/* 紫色 */
#define    F_CYAN 			"\033[36m" 			/* 青色 */
#define    F_WHITE 			"\033[37m" 			/* 白色 */
#define    F_LIGHTRED 		"\033[1;31m"    	/* 亮红 */
#define    F_LIGHTGREEN 	"\033[1;32m"    	/* 亮绿 */
#define    F_LIGHTYELLOW 	"\033[1;33m"    	/* 亮黄 */
#define    F_LIGHITBLUE 	"\033[1;34m"    	/* 亮蓝 */
#define    F_LIGHTMAGENTA	"\033[1;35m"    	/* 亮紫 */
#define    F_LIGHTCYAN 		"\033[1;36m"    	/* 亮青 */
#define    F_LIGHTWHITE 	"\033[1;37m"    	/* 亮白 */

/*背景色*/
#define    B_BLACK 			"\033[40m" 			/* 黑色 */
#define    B_RED 			"\033[41m" 			/* 红色 */
#define    B_GREEN 			"\033[42m" 			/* 绿色 */
#define    B_YELLOW 		"\033[43m" 			/* 黄色 */
#define    B_BLUE 			"\033[44m" 			/* 蓝色 */
#define    B_MAGENTA 		"\033[45m" 			/* 紫色 */
#define    B_CYAN 			"\033[46m" 			/* 青色 */
#define    B_LIGHTRED 		"\033[41;1m" 		/* 亮红 */
#define    B_LIGHTGREEN		"\033[42;1m" 		/* 亮绿 */
#define    B_LIGHTYELLOW 	"\033[43;1m" 		/* 亮黄 */
#define    B_LIGHTBLUE 		"\033[44;1m" 		/* 亮蓝 */
#define    B_LIGHTMAGENTA 	"\033[45;1m" 		/* 亮紫 */
#define    B_LIGHTCYAN 		"\033[46;1m" 		/* 亮青 */
#define    B_LIGHTWHITE 	"\033[47;1m" 		/* 亮白 */

#define    DEFAULT_F_Color			F_LIGHTGREEN	/* 默认前景色 */
#define    DEFAULT_B_Color			B_BLACK			/* 默认背景色 */
#define    PROMPT_F_Color			F_WHITE			/* 命令提示前景色 */

#define    BOLDFONT 		"\033[1m" 			/* Set blod font */
#define    UNDERLINEFONT 	"\033[4m" 			/* Set underline font */
#define    CLEARSCREEN 		"\033[2J" 			/* Clear screen */
#define    CURSORHOME 		"\033[H" 			/* Restore cursor to home */
#define    SAVECURSOR		"\033[s" 			/* Save cursor position */
#define    RESTORECURSOR	"\033[u" 			/* Restore cursor to saved position */
#define    SINGLEWIDTH 		"\033#5" 			/* Normal, single-width characters */
#define    DOUBLEWIDTH 		"\033#6" 			/* Creates double-width characters */

#define	   FREEZETOP		"\033[2;25r" 		/* Freeze top line */
#define    FREEZEBOTTOM		"\033[1;24r" 		/* Freeze bottom line */
#define    UNFREEZE_TB		"\033[r" 			/* Unfreeze top line and bottom line */

#define    BLINKTEXT		"\033[5m" 			/* Blink text */
#define    REVERSEMODE		"\033[7m" 			/* Set terminal to reverse mode */
#define    LIGHTREVERSEMODE	"\033[1,7m" 		/* Set terminal to light reverse mode */


/*****************************************************************************/
void InitTerminal(void);
void RunTerminal(void);

void SerialInterrupt(void);
void ParseArgs(char *argstr,char *argc_p,char **argv, char **resid);
void ExecCommand(char *buf);
/*****************************************************************************/

#endif