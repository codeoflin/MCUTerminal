//C语言代码在此:
#include "STC15F2K60S2.h"
#include "Uart.h"
#include "Terminal.h"

void main()
{
	P_SW1=0x40;//切换串口1到P36 P37
	UartInit();
	InitTerminal();
	while(1)
	{
		RunTerminal();
	}
}