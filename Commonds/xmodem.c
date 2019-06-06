#include "xmodem.h"

void xmodem()
{
    char run=1;
	char argc=Argc;
	const char **argv=Argv;
	//起点地址
	long int addr=0;
	//读写缓冲区
	char buff[0x400];
	char* example="xmodem w 0x8000";
    long int i=0;
    //包头
    char head=0;
    //包号
    unsigned char packno=0;
    //远程crc,本地crc
    unsigned int r_crc=0,l_crc=0;
    //包长
    int packlen=128;
	switch(argc)
	{
	case 3:
		if (strlen(argv[1])!=1)
		{
			outputError("参数1错误!",example);
			break;
		}
		addr=toLong(argv[2]);//解析地址
		if(argv[1][0]=='w'||argv[1][0]=='W')
		{
			sendLine("请开始传输文件...");
			waitFor('r');
			if(readByte()!='x')
			{
				sendLine("输入异常!");
				break;
			}
			waitFor(0xD);
            //一直循环发送'C'直到客户端开始发文件
            while(RI!=1)
            {
                for (i = 0; i < 10000; i+=2)i--;
                sendByte(NAK);
            }
            while(run)
            {
                head=readByte();
                switch(head)
                {
                case CAN:
                    sendLine("中断传输!");
                    run=0;
                    break;
                case EOT:
                    sendLine("发送结束!");
                    run=0;
                    break;
                case SOH://xmodem
                case STX://1k-xmodem
                    packlen=head==SOH?128:1024;
                    packno=readByte();
                    readByte();//包号反码,用于验证
                    for (i = 0; i < packlen; i++)buff[i]=readByte();                    
                    l_crc=checksum8(buff,packlen);
                    r_crc=readByte();
                    if (l_crc==r_crc)
                    {
                        for (i = 0; i < packlen; i++)
                        {
                            if(addr+i>=65536)
                            {
                                sendByte(CAN);
                                sendLine("数据超界!");
                                break;
                            }
                            writeFlash(addr+i,buff[i]);
                        }
                        addr+=packlen;
                        sendByte(ACK);
                    }else{
                        sendByte(NAK);
                    }
                    break;
                }
            }
			sendLine("操作完毕!");
		}
		break;
    default:
        outputError("参数数量错误!",example);
        break;
	}//End switch
}
