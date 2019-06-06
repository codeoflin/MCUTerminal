#ifndef  __EEPROM_H__
#define  __EEPROM_H__
#include "STC.h"
#include <stdio.h>
#include <string.h>
#include <intrins.h>
#include <ctype.h>
#define WT_30M  0x80
void writeFlash(unsigned int addr,unsigned char dat);
unsigned char readFlash(unsigned int addr);
void eraseFlash(unsigned int addr);
#endif