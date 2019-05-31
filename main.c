//C语言代码在此:
#include "STC15F2K60S2.h"
#include "Uart.h"
#include "Terminal.h"

void main()
{
	UartInit();
	InitTerminal();
	while(1)
	{
		RunTerminal();
	}
}