#include "Uart.h"
char code HEXTABLE[]={"0123456789ABCDEF"};

void UartInit(void)		//115200bps@30.000MHz
{
	SCON = 0x50;		//8位数据,可变波特率
	AUXR |= 0x40;		//定时器1时钟为Fosc,即1T
	AUXR &= 0xFE;		//串口1选择定时器1为波特率发生器
	TMOD &= 0x0F;		//设定定时器1为16位自动重装方式
	TL1 = 0xBF;		//设定定时初值
	TH1 = 0xFF;		//设定定时初值
	ET1 = 0;		//禁止定时器1中断
	TR1 = 1;		//启动定时器1
}

char readByte()
{
	char temp=0;
	while(RI==0);
	temp=SBUF;
	RI=0;
}
void waitFor(char c)
{
	while(readByte()!=c);
}
void sendByte(char dat)
{
	SBUF = dat;
	while(TI == 0);
	TI = 0;
}

void sendStr(char *str)
{
	if (str==0)return;
	
	while(*str != '\0')
	{
		sendByte(*str);
		str++;
	}
}

void sendLine(char *str)
{
	sendStr(str);
	sendStr("\r\n");
}

void sendStr2(char *str,char *fcolor,char *bcolor)
{
	sendStr(fcolor);
	sendStr(bcolor);
	sendStr(str);
	sendStr(DEFAULT_F_COLOR);
	sendStr(DEFAULT_B_COLOR);
}

void sendLine2(char *str,char *fcolor,char *bcolor)
{
	sendStr2(str,fcolor,bcolor);
	sendStr("\r\n");
}

void sendHexByte(unsigned char dat)
{
	unsigned char second=dat%0x10;
	sendByte(HEXTABLE[(dat-second)/0x10]);
	sendByte(HEXTABLE[second]);
	//sendByte(0x20);
}

/*
根据byte数组,转为16进制字符串发送
*/
void sendHexString(char *str,int lenght)
{
	int i=0;
	for(;i<lenght;i++)sendHexByte(str[i]);
}
/*
根据byte数组,转为16进制字符串发送,并自动按每16字节分行,每行在前面加4位地址
*/
void sendHexString2(char *str,unsigned int startaddr,unsigned int lenght)
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
			sendStr2(addrbuff,F_BLUE,DEFAULT_B_COLOR);
		}
		sendHexByte(str[i]);
		sendStr(i%0x10==0xF?"\r\n":" ");
	}
}

void sendUInt(unsigned int number)
{
	char buff[32];
	int i=0;
	do
	{
		buff[i]=number%10;
		number=(number-buff[i])/10;
		i++;
	}while(number>0);
	for (i--; i >=0; i--)sendByte(buff[i]+0x30);
}

void outputError(char* notice,char* example)
{
	sendStr(" 无效的操作: ");
	sendLine(notice);
	sendLine(" 例子:");
	sendStr("     ");
	sendLine(example);
}