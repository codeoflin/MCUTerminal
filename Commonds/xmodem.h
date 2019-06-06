#ifndef  __XMODEM_H__
#define  __XMODEM_H__
#include "../Devices/Flash.h"
#include "../Devices/Uart.h"
#include "../Commands.h"

#define SOH     0x01	//Xmodem数据头
#define STX     0x02	//1K-Xmodem数据头
#define EOT     0x04	//发送结束
#define ACK     0x06	//认可响应
#define NAK     0x15	//不认可响应
#define CAN     0x18	//撤销传送
#define CTRLZ   0x1A	//填充数据包

void xmodem();

#endif