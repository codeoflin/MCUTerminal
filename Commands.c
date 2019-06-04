#include "Commands.h"

char Argc;
char **Argv;

const COMMAND code CommandList[] = {
	{"help",help,"","帮助文档"},
	{"clear",clear,"","清屏"},
	{"reboot",reboot,"","重启"},
	{"flash",flash,"<mode> <address> <data|lenght>","读取EEPROM,参数:w或者r,地址,写的数据|读的长度."},
	{"setbit",setBit,"<BankID> <PinID>","设置IO口为1"},
	{"resetbit",resetBit,"<BankID> <PinID>","重置IO口为0"},
	{"getbit",getBit,"<BankID> <PinID>","读取IO口状态"},
	{NULL,NULL,NULL,NULL}
};
/*****************************************************************************/

void outpurError()
{

}

unsigned char readFlash(unsigned int addr)
{
	return *(char code *)(addr);
}

void flash()
{
	unsigned char dat=0;
	char argc=Argc;
	const char **argv=Argv;
	long int addr=0,len=0,i=0;
	char buff[0x40];
	switch(argc)
	{
	case 4:
		if (strlen(argv[1])!=1)
		{
			SendStr(" 无效的操作: 参数1错误.\r\n");
			SendStr(" 例子:\r\n");
			SendStr("     flash r 0 0\r\n");
			SendStr("     flash w 0 \"00 1F CD\"\r\n");
			break;
		}
	
		if(argv[1][0]=='r'||argv[1][0]=='R')
		{
			addr=toLong(argv[2]);
			len=toLong(argv[3]);
			//SendUInt(addr);
			//SendUInt(len);
			//break;
			if(addr<0||len<=0)
			{
				SendStr(" 无效的操作: 起点或长度错误!\r\n");
				SendStr(" 例子:\r\n");
				SendStr("     flash r 0 0\r\n");
				SendStr("     flash w 0 \"00 1F CD\"\r\n");
				break;
			}
			SendLine2("    |00 01 02 03 04 05 06 07 08 09 10 11 12 13 14 15",F_BLUE,DEFAULT_B_COLOR);
			SendLine2("    |00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F",F_BLUE,DEFAULT_B_COLOR);
			for(;len>0;)
			{
				for(i=0;i<0x40;i++)
				{
					if(i>=len)break;
					buff[i]=readFlash(addr+i);
				}
				SendHexString2(buff,addr,len>0x40?0x40:len);
				addr+=0x40;
				len-=0x40;
			}
		}
		SendStr("\r\n");
		break;
	default:
		SendStr(" 无效的操作: 参数数量不对.\r\n");
		SendStr(" 例子:\r\n");
		SendStr("     flash r 0 0\r\n");
		SendStr("     flash w 0 \"00 1F CD\"\r\n");
		break;
	}
}

void setBit()
{
	char argc=Argc;
	const char **argv=Argv;
	unsigned char arg2=0;
	unsigned char newstatus=0;
	switch(argc)
	{
	case 3:
		arg2=(unsigned char)argv[2][0]-0x30;
		if(arg2>9)
		{
			SendStr(" 无效的操作: 输入了非数字的参数!\r\n");
			SendStr(" 例子:\r\n");
			SendStr("     SetBit 1 1\r\n");
			break;
		}
		/* //注释原因:P30 P31可以不是串口
		if(argv[1][0]=='3'&&(arg2==0||arg2==1))
		{
			SendStr(" 无效的 'SetBit' 命令: P31 和 P30 is SerialPort!\r\n");
			SendStr(" Usage:\r\n");
			SendStr("     SetBit 1 1\r\n");
			break;
		}
		// */
		newstatus=_crol_(1,arg2);
		if(argv[1][0]=='0')P0|=newstatus;
		if(argv[1][0]=='1')P1|=newstatus;
		if(argv[1][0]=='2')P2|=newstatus;
		if(argv[1][0]=='3')P3|=newstatus;
		if(argv[1][0]=='4')P4|=newstatus;
		if(argv[1][0]=='5')P5|=newstatus;
		if(argv[1][0]=='6')P6|=newstatus;
		if(argv[1][0]=='7')P7|=newstatus;
		break;
	default:
		SendStr(" 无效的操作: 参数数量不对!\r\n");
		SendStr(" 例子:\r\n");
		SendStr("     SetBit 1 1\r\n");
		break;
	}
}

void resetBit()
{
	char argc=Argc;
	const char **argv=Argv;
	unsigned char arg2=0;
	unsigned char newstatus=0;
	switch(argc)
	{
	case 3:
		arg2=(unsigned char)argv[2][0]-0x30;
		if(arg2>9)
		{
			SendStr(" Invalid 'ResetBit' command: arguments not number!\r\n");
			SendStr(" Usage:\r\n");
			SendStr("     ResetBit 1 1\r\n");
			break;
		}
		if(argv[1][0]=='3'&&(arg2==0||arg2==1))
		{
			SendStr(" Invalid 'ResetBit' command: P31 and P30 is SerialPort!\r\n");
			SendStr(" Usage:\r\n");
			SendStr("     ResetBit 1 1\r\n");
			break;
		}
		newstatus=_crol_(0xFE,arg2);
		if(argv[1][0]=='0')P0&=newstatus;
		if(argv[1][0]=='1')P1&=newstatus;
		if(argv[1][0]=='2')P2&=newstatus;
		if(argv[1][0]=='3')P3&=newstatus;
		if(argv[1][0]=='4')P4&=newstatus;
		if(argv[1][0]=='5')P5&=newstatus;
		if(argv[1][0]=='6')P6&=newstatus;
		if(argv[1][0]=='7')P7&=newstatus;
		break;
	default:
		SendStr(" Invalid 'ResetBit' command: too many arguments\r\n");
		SendStr(" Example:\r\n");
		SendStr("     ResetBit 1 1\r\n");
		break;
	}
}

void getBit()
{
	char argc=Argc;
	const char **argv=Argv;
	unsigned char arg2=0;
	unsigned char status=0;
	
	//P2=0xF0;//测试数据
	//P3=0x0F;//测试数据
	switch(argc)
	{
	case 3:
		arg2=(unsigned char)argv[2][0]-0x30;
		if(arg2>7)
		{
			SendStr(" 无效的操作: 参数非数字或者大于7!\r\n");
			SendStr(" 例子:\r\n");
			SendStr("     GetBit 1 1\r\n");
			break;
		}
		if(argv[1][0]=='0')status=P0;
		if(argv[1][0]=='1')status=P1;
		if(argv[1][0]=='2')status=P2;
		if(argv[1][0]=='3')status=P3;
		if(argv[1][0]=='4')status=P4;
		if(argv[1][0]=='5')status=P5;
		if(argv[1][0]=='6')status=P6;
		if(argv[1][0]=='7')status=P7;

		//SendStr("数据:");
		//SerialSendHexByte(status);
		//SendStr("\r\n");
		SendStr((status&(unsigned char)_crol_(1,arg2))==0?"False":"True");
		SendStr("\r\n");

		break;
	default:
		SendStr(" 无效的操作: 参数数量不对\r\n");
		SendStr(" 例子:\r\n");
		SendStr("     GetBit 1 1\r\n");
		break;
	}
}

void help()
{
	char argc=Argc;
	const char **argv=Argv;
	char i;
	switch(argc)
	{
	case 1:
		for(i = 0; CommandList[i].HelpString!=NULL; i++)
		{
			SendStr(" ");
			SendStr2(CommandList[i].CommandName,F_RED,DEFAULT_B_COLOR);
			SendStr(" ");
			SendStr2(CommandList[i].Metadata,F_GREEN,DEFAULT_B_COLOR);
			SendStr2(" -- ",F_YELLOW,DEFAULT_B_COLOR);
			SendStr2(CommandList[i].HelpString,F_YELLOW,DEFAULT_B_COLOR);
			SendStr("\r\n");
		}
		SendStr("\r\n");
		break;
	default:
		SendStr(" 无效的操作: 参数过多\r\n");
		SendStr(" 例子:\r\n");
		SendStr("     help\r\n");
		break;
	}
}

void clear()
{
	char argc=Argc;
	const char **argv=Argv;
	switch(argc)
	{
	case 1:
		SendStr(CLEARSCREEN);	
		SendStr(CURSORHOME);
		break;
	default:
		SendStr(" 无效的操作: 参数过多!\r\n");
		SendStr(" 例子:\r\n");
		SendStr("     clear\r\n");
		break;
	}
}

void reboot()
{
	char argc=Argc;
	const char **argv=Argv;
	switch(argc)
	{
	case 1:
		(*(void(*)())0)(); 
		break;
	default:
		SendStr(" 无效的操作: 参数过多!\r\n");
		SendStr(" 例子:\r\n");
		SendStr("     reboot\r\n");
		break;
	}
}