#include "Commands.h"

char Argc;
char **Argv;

const COMMAND code CommandList[] = {
	{"help",Help,"","帮助文档"},
	{"clear",Clear,"","清屏"},
	{"reboot",Reboot,"","重启"},
	{"setbit",SetBit,"<BankID> <PinID>","设置IO口为1"},
	{"resetbit",ResetBit,"<BankID> <PinID>","重置IO口为0"},
	{"getbit",GetBit,"<BankID> <PinID>","读取IO口状态"},
	{NULL,NULL,NULL,NULL}
};
/*****************************************************************************/
void SetBit()
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
			SendStr(" Invalid 'SetBit' command: arguments not number!\r\n");
			SendStr(" Usage:\r\n");
			SendStr("     SetBit 1 1\r\n");
			break;
		}
		if(argv[1][0]=='3'&&(arg2==0||arg2==1))
		{
			SendStr(" Invalid 'SetBit' command: P31 and P30 is SerialPort!\r\n");
			SendStr(" Usage:\r\n");
			SendStr("     SetBit 1 1\r\n");
			break;
		}
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
		SendStr(" Invalid 'SetBit' command: too many arguments\r\n");
		SendStr(" Example:\r\n");
		SendStr("     SetBit 1 1\r\n");
		break;
	}
}

void ResetBit()
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

void GetBit()
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
			SendStr(" Invalid 'GetBit' command: arguments not number or over 7!\r\n");
			SendStr(" Usage:\r\n");
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
		SendStr(" Invalid 'GetBit' command: too many arguments\r\n");
		SendStr(" Example:\r\n");
		SendStr("     GetBit 1 1\r\n");
		break;
	}
}

void Help()
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

void Clear()
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
		SendStr(" Invalid 'clear' command: too many arguments\r\n");
		SendStr(" Usage:\r\n");
		SendStr("     clear\r\n");
		break;
	}
}

void Reboot()
{
	char argc=Argc;
	const char **argv=Argv;
	switch(argc)
	{
	case 1:
		(*(void(*)())0)(); 
		break;
	default:
		SendStr(" Invalid 'reboot' command: too many arguments\r\n");
		SendStr(" Usage:\r\n");
		SendStr("     reboot\r\n");
		break;
	}
}