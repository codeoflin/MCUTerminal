#include "Run51.h"

void run(long int addr,long int len)
{
    while(1)
    {
        readFlash(addr);
    }
}

void run51()
{
	char argc=Argc;
	const char **argv=Argv;
	//起点地址,要读写的长度
	long int addr=0,len=0;
	int i=0;
	//输入命令有错误的时候用这个提示用户输入正确的命令
	char* example="run51 0x8000 0x400";
	switch(argc)
	{
	case 3:
		addr=toLong(argv[1]);//解析地址
        len=toLong(argv[2]);//解析长度
        if(addr<0||len<=0)
        {
		    outputError("请输入正确的起始地址与长度!",example);
            break;
        }
        run(addr,len);
		break;
	default:
		outputError("参数数量不对!",example);
		break;
	}
}