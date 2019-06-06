#ifndef  __RUN51_H__
#define  __RUN51_H__

#include "../Devices/EEPROM.h"
#include "../Devices/Uart.h"
#include "../Commands.h"

void run51();

#define ASM_MOV_NUM_MEM         0x85        //MOV MEM,NUM
#define ASM_SJMP_NUM            0x80        //SJMP NUM
#define ASM_NOP                 0x00        //NOP NUM

#endif