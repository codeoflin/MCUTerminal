#include "Commands.h"

char Argc;
char **Argv;

const COMMAND code CommandList[] = {
	{"help",help,"","帮助文档"},
	{"clear",clear,"","清屏"},
	{"reboot",reboot,"","重启"},
	{"flash",flash,"<mode> <address> <data|lenght>","EEPROM操作,参数:w|r|e,地址,写的数据|读的长度|(擦除时免传此参数)."},
	{"xmodem",xmodem,"<mode> <address> <lenght>","通过xmodem传输文件,参数:w|r,地址,读的长度|(写时免传此参数)."},
	{"setbit",setBit,"<BankID> <PinID>","设置IO口为1"},
	{"resetbit",resetBit,"<BankID> <PinID>","重置IO口为0"},
	{"getbit",getBit,"<BankID> <PinID>","读取IO口状态"},
	{"run51",run51,"<address> <lenght>","运行51程序,参数是flash的起点与长度,此处选中的数据将映射到虚拟机里的flash起点"},
	{NULL,NULL,NULL,NULL}
};
/*****************************************************************************/

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
			outputError("输入了非数字的参数!","SetBit 1 1");
			break;
		}
		/* //注释原因:P30 P31可以不是串口
		if(argv[1][0]=='3'&&(arg2==0||arg2==1))
		{
			sendStr(" 无效的 'SetBit' 命令: P31 和 P30 is SerialPort!\r\n");
			sendStr(" Usage:\r\n");
			sendStr("     SetBit 1 1\r\n");
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
		outputError("参数数量不对!","SetBit 1 1");
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
			outputError("请输入数字参数!","ResetBit 1 1");
			break;
		}
		if(argv[1][0]=='3'&&(arg2==0||arg2==1))
		{
			outputError("P31 and P30 is SerialPort!","ResetBit 1 1");
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
		outputError("参数数量不对!","ResetBit 1 1");
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
			outputError("参数非数字或者大于7!","GetBit 1 1");
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
		sendLine((status&(unsigned char)_crol_(1,arg2))==0?"False":"True");
		break;
	default:
		outputError("参数数量不对!","GetBit 1 1");
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
			sendStr(" ");
			sendStr2(CommandList[i].CommandName,F_RED,DEFAULT_B_COLOR);
			sendStr(" ");
			sendStr2(CommandList[i].Metadata,F_GREEN,DEFAULT_B_COLOR);
			sendStr2(" -- ",F_YELLOW,DEFAULT_B_COLOR);
			sendLine2(CommandList[i].HelpString,F_YELLOW,DEFAULT_B_COLOR);
		}
		sendLine(NULL);
		break;
	default:
		outputError("参数过多!","help");
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
		sendStr(CLEARSCREEN);	
		sendStr(CURSORHOME);
		break;
	default:
		outputError("参数过多!","clear");
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
		//(*(void(*)())0)(); 
		IAP_CONTR = 0x60;
		break;
	default:
		outputError("参数过多!","reboot");
		break;
	}
}