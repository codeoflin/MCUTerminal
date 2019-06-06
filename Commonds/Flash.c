#include "Flash.h"

void flash()
{
	char argc=Argc;
	const char **argv=Argv;
	//起点地址,要读写的长度
	long int addr=0,len=0;
	int i=0;
	//读写缓冲区
	char buff[0x40];
	//输入命令有错误的时候用这个提示用户输入正确的命令
	char* example="flash r 0 0\r\n     flash w 0 \"00 1F CD\"\r\n     flash e 0";
	switch(argc)
	{
	case 3:
		if (strlen(argv[1])!=1)
		{
			outputError("参数1错误!",example);
			break;
		}
		addr=toLong(argv[2]);//解析地址
		if(argv[1][0]=='e'||argv[1][0]=='E')
		{
			eraseFlash((unsigned int)addr);
			sendLine("操作完毕!");
		}
		break;
	case 4:
		if (strlen(argv[1])!=1)
		{
			outputError("参数1错误!",example);
			break;
		}
		addr=toLong(argv[2]);//解析地址
		if(argv[1][0]=='r'||argv[1][0]=='R')
		{
			len=toLong(argv[3]);//解析长度
			if(addr<0||len<=0)
			{
				outputError("起点或长度错误!",example);
				break;
			}
			sendLine2("    |00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F",F_BLUE,DEFAULT_B_COLOR);
			//循环读取输出
			for(;len>0;)
			{
				for(i=0;i<0x40;i++)
				{
					if(i>=len)break;
					buff[i]=readFlash(addr+i);
				}
				sendHexString2(buff,addr,len>0x40?0x40:len);
				addr+=0x40;
				len-=0x40;
			}
		}

		if(argv[1][0]=='w'||argv[1][0]=='W')
		{
			len=parseHex(argv[3],buff,0x40);
			if(len<=0)
			{
				outputError("请输入正确的HEX字符串!",example);
				break;
			}
			for(i=0;i<len;i++)
			{
				writeFlash(addr+i,buff[i]);
			}
			sendLine("操作完毕!");
		}
		sendLine(NULL);
		break;
	default:
		outputError("参数数量不对!",example);
		break;
	}
}