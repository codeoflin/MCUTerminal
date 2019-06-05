#include "Flash.h"
unsigned char readFlash(unsigned int addr)
{
	return *(char code *)(addr);
}

void writeFlash(unsigned int addr,unsigned char dat)
{
	IAP_CONTR=WT_30M;
	IAP_CMD=2;//写命令
	IAP_ADDRL=addr;
	IAP_ADDRH=addr>>8;
	IAP_DATA=dat;
	IAP_TRIG=0x5A;
	IAP_TRIG=0xA5;
	_nop_();
	IAP_CONTR=0;//控制寄存器
	IAP_CMD=0;//命令寄存器
	IAP_TRIG=0;//触发寄存器
	IAP_ADDRH=0;
	IAP_ADDRL=0;
	return;
}

void eraseFlash(unsigned int addr)
{
	IAP_CONTR=WT_30M;
	IAP_CMD=3;//擦命令
	IAP_ADDRL=addr;
	IAP_ADDRH=addr>>8;
	IAP_TRIG=0x5A;
	IAP_TRIG=0xA5;
	_nop_();
	IAP_CONTR=0;//控制寄存器
	IAP_CMD=0;//命令寄存器
	IAP_TRIG=0;//触发寄存器
	IAP_ADDRH=0;
	IAP_ADDRL=0;
	return;
}