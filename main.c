//C语言代码在此:
#include "STC.h"
#include "Uart.h"
#include "Terminal.h"

void main()
{
	P_SW1=0x40;//切换串口1到P36 P37
	UartInit();
	REN=1;//设定允许接收串口数据
	EA=0;
	ES=0;
	InitTerminal();
	while(1)
	{
		//SendByte(0x30);
		RunTerminal();
	}
}