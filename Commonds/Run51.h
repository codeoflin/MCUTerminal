#ifndef  __RUN51_H__
#define  __RUN51_H__

#include "../Devices/EEPROM.h"
#include "../Devices/Uart.h"
#include "../Commands.h"
void run51();
unsigned char readREG_Rx(unsigned char id);

//指令机器码
typedef enum {
	ASM_MOV_R7_A					=0xFF,				//
	ASM_MOV_R6_A					=0xFE,				//
	ASM_MOV_R5_A					=0xFD,				//
	ASM_MOV_R4_A					=0xFC,				//
	ASM_MOV_R3_A					=0xFB,				//
	ASM_MOV_R2_A					=0xFA,				//
	ASM_MOV_R1_A					=0xF9,				//
	ASM_MOV_R0_A					=0xF8,				//
	ASM_MOV_XR1_A					=0xF7,				//
	ASM_MOV_XR0_A					=0xF6,				//
	ASM_MOV_DIRECT_A			=0xF5,				//
	ASM_CPL_A							=0xF4,				//取反
	ASM_MOVX_XR1_A				=0xF3,				//MOVX @Ri,A
	ASM_MOVX_XR0_A				=0xF2,				//MOVX @Ri,A
	ASM_MOVX_XDPTR_A			=0xF0,				//MOVX @DPTR,A
	ASM_MOV_A_R7					=0xEF,				//
	ASM_MOV_A_R6					=0xEE,				//
	ASM_MOV_A_R5					=0xED,				//
	ASM_MOV_A_R4					=0xEC,				//
	ASM_MOV_A_R3					=0xEB,				//
	ASM_MOV_A_R2					=0xEA,				//
	ASM_MOV_A_R1					=0xE9,				//
	ASM_MOV_A_R0					=0xE8,				//
	ASM_MOV_A_XR1					=0xE7,				//MOV A,@Ri
	ASM_MOV_A_XR0					=0xE6,				//MOV A,@Ri
	ASM_MOV_A_DIRECT			=0xE5,				//
	ASM_CLR_A							=0xE4,				//清0
	ASM_MOVX_A_XR1				=0xE3,				//MOVX A,@Ri
	ASM_MOVX_A_XR0				=0xE2,				//MOVX A,@Ri
	ASM_MOVX_A_XDPTR			=0xE0,				//MOVX A,@DPTR
	ASM_DJNZ_R7_REL				=0xDF,				//
	ASM_DJNZ_R6_REL				=0xDE,				//
	ASM_DJNZ_R5_REL				=0xDD,				//
	ASM_DJNZ_R4_REL				=0xDC,				//
	ASM_DJNZ_R3_REL				=0xDB,				//
	ASM_DJNZ_R2_REL				=0xDA,				//
	ASM_DJNZ_R1_REL				=0xD9,				//
	ASM_DJNZ_R0_REL				=0xD8,				//
	ASM_XCHD_A_XR1				=0xD7,				//@R1和A交换低4位
	ASM_XCHD_A_XR0				=0xD6,				//@R0和A交换低4位
	ASM_DJNZ_DIRECT_REL		=0xD5,				//
	ASM_DA_A							=0xD4,				//十进制整理指令
	ASM_SETB_C						=0xD3,				//CY置1
	ASM_SETB_BIT					=0xD2,				//位置1
	ASM_POP_DIRECT				=0xD0,				//
	ASM_XCH_A_R7					=0xCF,				//互换
	ASM_XCH_A_R6					=0xCE,				//互换
	ASM_XCH_A_R5					=0xCD,				//互换
	ASM_XCH_A_R4					=0xCC,				//互换
	ASM_XCH_A_R3					=0xCB,				//互换
	ASM_XCH_A_R2					=0xCA,				//互换
	ASM_XCH_A_R1					=0xC9,				//互换
	ASM_XCH_A_R0					=0xC8,				//互换
	ASM_XCH_A_XR1					=0xC7,				//互换
	ASM_XCH_A_XR0					=0xC6,				//互换
	ASM_XCH_A_DIRECT			=0xC5,				//互换
	ASM_SWAP_A						=0xC4,				//A高低4位互换
	ASM_CLR_C							=0xC3,				//CY清0
	ASM_CLR_BIT						=0xC2,				//位清0
	ASM_PUSH_DIRECT				=0xC0,				//内存入栈
	ASM_CJNE_R7_DATA_REL	=0xBF,				//
	ASM_CJNE_R6_DATA_REL	=0xBE,				//
	ASM_CJNE_R5_DATA_REL	=0xBD,				//
	ASM_CJNE_R4_DATA_REL	=0xBC,				//
	ASM_CJNE_R3_DATA_REL	=0xBB,				//
	ASM_CJNE_R2_DATA_REL	=0xBA,				//
	ASM_CJNE_R1_DATA_REL	=0xB9,				//
	ASM_CJNE_R0_DATA_REL	=0xB8,				//
	ASM_CJNE_XR1_DATA_REL	=0xB7,				//
	ASM_CJNE_XR0_DATA_REL	=0xB6,				//
	ASM_CJNE_A_DIRECT_REL	=0xB5,				//
	ASM_CJNE_A_DATA_REL		=0xB4,				//
	ASM_CPL_C							=0xB3,				//翻转CY
	ASM_CPL_Bit						=0xB2,				//翻转Bit
	ASM_ANL_C_NBIT				=0xB0,				//
	ASM_MOV_R7_DIRECT			=0xAF,				//
	ASM_MOV_R6_DIRECT			=0xAE,				//
	ASM_MOV_R5_DIRECT			=0xAD,				//
	ASM_MOV_R4_DIRECT			=0xAC,				//
	ASM_MOV_R3_DIRECT			=0xAB,				//
	ASM_MOV_R2_DIRECT			=0xAA,				//
	ASM_MOV_R1_DIRECT			=0xA9,				//
	ASM_MOV_R0_DIRECT			=0xA8,				//
	ASM_MOV_XR1_DIRECT		=0xA7,				//
	ASM_MOV_XR0_DIRECT		=0xA6,				//
	ASM_MUL_AB						=0xA4,				//乘法
	ASM_INC_DPTR					=0xA3,				//
	ASM_MOV_C_BIT					=0xA2,				//MOV C,Bit
	ASM_ORL_C_NBIT				=0xA0,				//
	ASM_SUBB_A_R7		  		=0x9F,				//
	ASM_SUBB_A_R6		  		=0x9E,				//
	ASM_SUBB_A_R5		  		=0x9D,				//
	ASM_SUBB_A_R4		  		=0x9C,				//
	ASM_SUBB_A_R3		  		=0x9B,				//
	ASM_SUBB_A_R2		  		=0x9A,				//
	ASM_SUBB_A_R1		  		=0x99,				//
	ASM_SUBB_A_R0		  		=0x98,				//
	ASM_SUBB_A_XR1		  	=0x97,				//
	ASM_SUBB_A_XR0		  	=0x96,				//
	ASM_SUBB_A_DIRECT			=0x95,				//
	ASM_SUBB_A_DATA				=0x94,				//
	ASM_MOVC_A_X_A_DPTR		=0x93,				//MOVC A,@A+DPTR
	ASM_MOV_BIT_C					=0x92,				//MOV Bit,C
	ASM_MOV_DPTR_DATA16		=0x90,				//立即数写入内存
	ASM_MOV_DIRECT_R7			=0x8F,				//
	ASM_MOV_DIRECT_R6			=0x8E,				//
	ASM_MOV_DIRECT_R5			=0x8D,				//
	ASM_MOV_DIRECT_R4			=0x8C,				//
	ASM_MOV_DIRECT_R3			=0x8B,				//
	ASM_MOV_DIRECT_R2			=0x8A,				//
	ASM_MOV_DIRECT_R1			=0x89,				//
	ASM_MOV_DIRECT_R0			=0x88,				//
	ASM_MOV_DIRECT_XR1		=0x87,				//
	ASM_MOV_DIRECT_XR0		=0x86,				//
	ASM_MOV_DIRECT_DIRECT	=0x85,				//内存直接转移
	ASM_DIV_AB						=0x84,				//除法
	ASM_MOVC_A_X_A_PC			=0x83,				//MOVC A,@A+PC
	ASM_ANL_C_BIT					=0x82,				//
	ASM_SJMP_REL					=0x80,        //
	ASM_MOV_R7_DATA				=0x7F,				//立即数写入R7
	ASM_MOV_R6_DATA				=0x7E,				//立即数写入R6
	ASM_MOV_R5_DATA				=0x7D,				//立即数写入R5
	ASM_MOV_R4_DATA				=0x7C,				//立即数写入R4
	ASM_MOV_R3_DATA				=0x7B,				//立即数写入R3
	ASM_MOV_R2_DATA				=0x7A,				//立即数写入R2
	ASM_MOV_R1_DATA				=0x79,				//立即数写入R1
	ASM_MOV_R0_DATA				=0x78,				//立即数写入R0
	ASM_MOV_XR1_DATA			=0x77,				//
	ASM_MOV_XR0_DATA			=0x76,				//
	ASM_MOV_DIRECT_DATA		=0x75,				//立即数写入内存
	ASM_MOV_A_DATA				=0x74,				//立即数写入A
	ASM_JMP_XA_DPTR				=0x73,				//
	ASM_ORL_C_BIT					=0x72,				//
	ASM_JNZ_REL						=0x70,				//
	ASM_XRL_A_R7		  		=0x6F,				//
	ASM_XRL_A_R6		  		=0x6E,				//
	ASM_XRL_A_R5		  		=0x6D,				//
	ASM_XRL_A_R4		  		=0x6C,				//
	ASM_XRL_A_R3		  		=0x6B,				//
	ASM_XRL_A_R2		  		=0x6A,				//
	ASM_XRL_A_R1		  		=0x69,				//
	ASM_XRL_A_R0		  		=0x68,				//
	ASM_XRL_A_XR1		  		=0x67,				//
	ASM_XRL_A_XR0		  		=0x66,				//
	ASM_XRL_A_DIRECT			=0x65,				//
	ASM_XRL_A_DATA				=0x64,				//
	ASM_XRL_DIRECT_DATA		=0x62,				//
	ASM_JZ_REL						=0x60,				//
	ASM_ANL_A_R7		  		=0x5F,				//
	ASM_ANL_A_R6		  		=0x5E,				//
	ASM_ANL_A_R5		  		=0x5D,				//
	ASM_ANL_A_R4		  		=0x5C,				//
	ASM_ANL_A_R3		  		=0x5B,				//
	ASM_ANL_A_R2		  		=0x5A,				//
	ASM_ANL_A_R1		  		=0x59,				//
	ASM_ANL_A_R0		  		=0x58,				//
	ASM_ANL_A_XR1		  		=0x57,				//
	ASM_ANL_A_XR0		  		=0x56,				//
	ASM_ANL_A_DIRECT			=0x55,				//
	ASM_ANL_A_DATA				=0x54,				//
	ASM_ANL_DIRECT_DATA		=0x52,				//
	ASM_JNC_REL			  		=0x50,				//
	ASM_ORL_A_R7		  		=0x4F,				//
	ASM_ORL_A_R6		  		=0x4E,				//
	ASM_ORL_A_R5		  		=0x4D,				//
	ASM_ORL_A_R4		  		=0x4C,				//
	ASM_ORL_A_R3		  		=0x4B,				//
	ASM_ORL_A_R2		  		=0x4A,				//
	ASM_ORL_A_R1		  		=0x49,				//
	ASM_ORL_A_R0		  		=0x48,				//
	ASM_ORL_A_XR1		  		=0x47,				//
	ASM_ORL_A_XR0		  		=0x46,				//
	ASM_ORL_A_DIRECT			=0x45,				//
	ASM_ORL_A_DATA				=0x44,				//
	ASM_ORL_DIRECT_DATA		=0x43,				//
	ASM_ORL_DIRECT_A			=0x42,				//
	ASM_JC_REL			  		=0x40,				//
	ASM_ADDC_A_R7		  		=0x3F,				//
	ASM_ADDC_A_R6		  		=0x3E,				//
	ASM_ADDC_A_R5		  		=0x3D,				//
	ASM_ADDC_A_R4		  		=0x3C,				//
	ASM_ADDC_A_R3		  		=0x3B,				//
	ASM_ADDC_A_R2		  		=0x3A,				//
	ASM_ADDC_A_R1		  		=0x39,				//
	ASM_ADDC_A_R0		  		=0x38,				//
	ASM_ADDC_A_XR1		  	=0x37,				//
	ASM_ADDC_A_XR0		  	=0x36,				//
	ASM_ADDC_A_DIRECT			=0x35,				//
	ASM_ADDC_A_DATA				=0x34,				//
	ASM_RLC_A							=0x33,				//
	ASM_RETI							=0x32,				//
	ASM_JNB_BIT_REL  			=0x30,				//
	ASM_ADD_A_R7		  		=0x2F,				//
	ASM_ADD_A_R6		  		=0x2E,				//
	ASM_ADD_A_R5		  		=0x2D,				//
	ASM_ADD_A_R4		  		=0x2C,				//
	ASM_ADD_A_R3		  		=0x2B,				//
	ASM_ADD_A_R2		  		=0x2A,				//
	ASM_ADD_A_R1		  		=0x29,				//
	ASM_ADD_A_R0		  		=0x28,				//
	ASM_ADD_A_XR1		  		=0x27,				//
	ASM_ADD_A_XR0		  		=0x26,				//
	ASM_ADD_A_DIRECT			=0x25,				//
	ASM_ADD_A_DATA				=0x24,				//
	ASM_RL_A							=0x23,				//
	ASM_RET								=0x22,				//
	ASM_JB_BIT_REL  			=0x20,				//
	ASM_DEC_R7		  			=0x1F,				//
	ASM_DEC_R6		  			=0x1E,				//
	ASM_DEC_R5		  			=0x1D,				//
	ASM_DEC_R4		  			=0x1C,				//
	ASM_DEC_R3		  			=0x1B,				//
	ASM_DEC_R2		  			=0x1A,				//
	ASM_DEC_R1		  			=0x19,				//
	ASM_DEC_R0		  			=0x18,				//
	ASM_DEC_XR1		  			=0x17,				//
	ASM_DEC_XR0		  			=0x16,				//
	ASM_DEC_DIRECT				=0x15,				//
	ASM_DEC_A							=0x14,				//
	ASM_RRC_A							=0x13,				//
	ASM_LCALL_ADDR16	  	=0x12,				//
	ASM_JBC_BIT_REL		  	=0x10,				//
	ASM_INC_R7		  			=0x0F,				//
	ASM_INC_R6		  			=0x0E,				//
	ASM_INC_R5		  			=0x0D,				//
	ASM_INC_R4		  			=0x0C,				//
	ASM_INC_R3		  			=0x0B,				//
	ASM_INC_R2		  			=0x0A,				//
	ASM_INC_R1		  			=0x09,				//
	ASM_INC_R0		  			=0x08,				//
	ASM_INC_XR1		  			=0x07,				//
	ASM_INC_XR0		  			=0x06,				//
	ASM_INC_DIRECT				=0x05,				//
	ASM_INC_A							=0x04,				//
	ASM_RR_A							=0x03,				//
	ASM_LJMP_ADDR16				=0x02,				//
	ASM_NOP								=0x00,        //
	ASM_ACALL_111_ADDR11	=0xF1,				//
	ASM_ACALL_110_ADDR11	=0xD1,				//
	ASM_ACALL_101_ADDR11	=0xB1,				//
	ASM_ACALL_100_ADDR11	=0x91,				//
	ASM_ACALL_011_ADDR11	=0x71,				//
	ASM_ACALL_010_ADDR11	=0x51,				//
	ASM_ACALL_001_ADDR11	=0x31,				//
	ASM_ACALL_000_ADDR11	=0x11,				//
	ASM_AJMP_111_ADDR11		=0xE1,				//
	ASM_AJMP_110_ADDR11		=0xC1,				//
	ASM_AJMP_101_ADDR11		=0xA1,				//
	ASM_AJMP_100_ADDR11		=0x81,				//
	ASM_AJMP_011_ADDR11		=0x61,				//
	ASM_AJMP_010_ADDR11		=0x41,				//
	ASM_AJMP_001_ADDR11		=0x21,				//
	ASM_AJMP_000_ADDR11		=0x01					//
}MACHINECODE;

//寄存器
#define REG_P0                  0x80        //P0
#define REG_SP                  0x81        //堆栈指针
#define REG_DPL                 0x82        //数据指针DPTR低8位
#define REG_DPH                 0x83        //数据指针DPTR高8位
#define REG_P1                  0x90        //P1
#define REG_P2                  0xA0        //P2
#define REG_P3                  0xB0        //P3
#define REG_PSW                 0xD0        //程序状态字寄存器
#define REG_ACC                 0xE0        //ACC累加寄存器
#define REG_B                   0xF0        //B

//位寻址
#define BIT_CY									0xD7


#endif