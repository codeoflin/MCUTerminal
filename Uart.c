#include "Uart.h"

void SendByte(char dat)
{
	SBUF = dat;
	while(TI == 0);
	TI = 0;
}

void SendStr(char *str)
{
	if (str==0)return;
	
	while(*str != '\0')
	{
		SendByte(*str);
		str++;
	}
}

void SendLine(char *str)
{
	SendStr(str);
	SendStr("\r\n");
}

void SendStr2(char *str,char *fcolor,char *bcolor)
{
	SendStr(fcolor);
	SendStr(bcolor);
	SendStr(str);
	SendStr(DEFAULT_F_COLOR);
	SendStr(DEFAULT_B_COLOR);
}

void SendLine2(char *str,char *fcolor,char *bcolor)
{
	SendStr2(str,fcolor,bcolor);
	SendStr("\r\n");
}

void SendHexByte(unsigned char dat)
{
	unsigned char second=dat%0x10;
	SendByte("0123456789ABCDEF"[(dat-second)/0x10]);
	SendByte("0123456789ABCDEF"[second]);
	SendByte(0x20);
}

void SendHexString(char *str,int lenght)
{
	int i=0;
	for(;i<lenght;i++)SendHexByte(str[i]);
}

void UartInit(void)		//115200bps@30.000MHz
{
	PCON &= 0x7F;		//波特率不倍速
	SCON = 0x50;		//8位数据,可变波特率
	AUXR |= 0x40;		//定时器1时钟为Fosc,即1T
	AUXR &= 0xFE;		//串口1选择定时器1为波特率发生器
	TMOD &= 0x0F;		//清除定时器1模式位
	TMOD |= 0x20;		//设定定时器1为8位自动重装方式
	TL1 = 0xF8;		//设定定时初值
	TH1 = 0xF8;		//设定定时器重装值
	ET1 = 0;		//禁止定时器1中断
	TR1 = 1;		//启动定时器1
}

void SendUInt(unsigned int number)
{
	char buff[32];
	int i=0;
	do
	{
		buff[i]=number%10;
		number=(number-buff[i])/10;
		i++;
	}while(number>0);
	for (i--; i >=0; i--)SendByte(buff[i]+0x30);
}