#include "Uart.h"
char code HEXTABLE[]={"0123456789ABCDEF"};

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
	SendByte(HEXTABLE[(dat-second)/0x10]);
	SendByte(HEXTABLE[second]);
	//SendByte(0x20);
}

/*
根据byte数组,转为16进制字符串发送
*/
void SendHexString(char *str,int lenght)
{
	int i=0;
	for(;i<lenght;i++)SendHexByte(str[i]);
}
/*
根据byte数组,转为16进制字符串发送,并自动按每16字节分行,每行在前面加4位地址
*/
void SendHexString2(char *str,unsigned int startaddr,unsigned int lenght)
{
	long int  i=0,l;
	for(;i<lenght;i++)
	{
		if(i%0x10==0)
		{
			unsigned int  newaddr=startaddr+i;
			char addrbuff[]={0,0,0,0,0,0};
			addrbuff[4]='|';
			addrbuff[3]=HEXTABLE[newaddr%0x10];
			addrbuff[2]=HEXTABLE[(newaddr>>4)%0x10];
			addrbuff[1]=HEXTABLE[(newaddr>>8)%0x10];
			addrbuff[0]=HEXTABLE[(newaddr>>12)%0x10];
			SendStr2(addrbuff,F_BLUE,DEFAULT_B_COLOR);
		}
		SendHexByte(str[i]);
		SendStr(i%0x10==0xF?"\r\n":" ");
	}
}

void UartInit(void)		//460800bps@30.000MHz
{
	SCON = 0x50;		//8位数据,可变波特率
	AUXR |= 0x40;		//定时器1时钟为Fosc,即1T
	AUXR &= 0xFE;		//串口1选择定时器1为波特率发生器
	TMOD &= 0x0F;		//设定定时器1为16位自动重装方式
	TL1 = 0xF0;		//设定定时初值
	TH1 = 0xFF;		//设定定时初值
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