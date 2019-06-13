#ifndef  __RUN51_H__
#define  __RUN51_H__

#include "../Devices/EEPROM.h"
#include "../Devices/Uart.h"
#include "../Commands.h"

void run51();
unsigned char readREG_Rx(unsigned char id);

//指令机器码
#define ASM_MOV_R7_A					  0xFF				//
#define ASM_MOV_R6_A					  0xFE				//
#define ASM_MOV_R5_A					  0xFD				//
#define ASM_MOV_R4_A					  0xFC				//
#define ASM_MOV_R3_A					  0xFB				//
#define ASM_MOV_R2_A					  0xFA				//
#define ASM_MOV_R1_A					  0xF9				//
#define ASM_MOV_R0_A					  0xF8				//
#define ASM_MOV_XR1_A					  0xF7				//
#define ASM_MOV_XR0_A					  0xF6				//
#define ASM_MOV_DIRECT_A			  0xF5				//
#define ASM_CPL_A							  0xF4				//取反
#define ASM_MOVX_XR1_A					0xF3				//MOVX @Ri,A
#define ASM_MOVX_XR0_A					0xF2				//MOVX @Ri,A
#define ASM_MOVX_XDPTR_A				0xF0				//MOVX @DPTR,A
#define ASM_MOV_A_R7					  0xEF				//
#define ASM_MOV_A_R6					  0xEE				//
#define ASM_MOV_A_R5					  0xED				//
#define ASM_MOV_A_R4					  0xEC				//
#define ASM_MOV_A_R3					  0xEB				//
#define ASM_MOV_A_R2					  0xEA				//
#define ASM_MOV_A_R1					  0xE9				//
#define ASM_MOV_A_R0					  0xE8				//
#define ASM_MOV_A_XR1					  0xE7				//MOV A,@Ri
#define ASM_MOV_A_XR0					  0xE6				//MOV A,@Ri
#define ASM_MOV_A_DIRECT			  0xE5				//
#define ASM_CLR_A							  0xE4				//清0
#define ASM_MOVX_A_XR1					0xE3				//MOVX A,@Ri
#define ASM_MOVX_A_XR0					0xE2				//MOVX A,@Ri
#define ASM_MOVX_A_XDPTR			  0xE0				//MOVX A,@DPTR
#define ASM_DJNZ_R7_REL					0xDF				//
#define ASM_DJNZ_R6_REL					0xDE				//
#define ASM_DJNZ_R5_REL					0xDD				//
#define ASM_DJNZ_R4_REL					0xDC				//
#define ASM_DJNZ_R3_REL					0xDB				//
#define ASM_DJNZ_R2_REL					0xDA				//
#define ASM_DJNZ_R1_REL					0xD9				//
#define ASM_DJNZ_R0_REL					0xD8				//
#define ASM_XCHD_A_XR1					0xD7				//@R1和A交换低4位
#define ASM_XCHD_A_XR0					0xD6				//@R0和A交换低4位
#define ASM_DJNZ_DIRECT_REL			0xD5				//
#define ASM_DA_A								0xD4				//十进制整理指令
#define ASM_SETB_C							0xD3				//CY置1
#define ASM_SETB_BIT					  0xD2				//位置1
#define ASM_POP_DIRECT					0xD0				//
#define ASM_XCH_A_R7						0xCF				//互换
#define ASM_XCH_A_R6						0xCE				//互换
#define ASM_XCH_A_R5						0xCD				//互换
#define ASM_XCH_A_R4						0xCC				//互换
#define ASM_XCH_A_R3						0xCB				//互换
#define ASM_XCH_A_R2						0xCA				//互换
#define ASM_XCH_A_R1						0xC9				//互换
#define ASM_XCH_A_R0						0xC8				//互换
#define ASM_XCH_A_XR1						0xC7				//互换
#define ASM_XCH_A_XR0						0xC6				//互换
#define ASM_XCH_A_DIRECT				0xC5				//互换
#define ASM_SWAP_A							0xC4				//A高低4位互换
#define ASM_CLR_C								0xC3				//CY清0
#define ASM_CLR_BIT							0xC2				//位清0
#define ASM_PUSH_DIRECT					0xC0				//内存入栈
#define ASM_CJNE_R7_DATA_REL		0xBF				//
#define ASM_CJNE_R6_DATA_REL		0xBE				//
#define ASM_CJNE_R5_DATA_REL		0xBD				//
#define ASM_CJNE_R4_DATA_REL		0xBC				//
#define ASM_CJNE_R3_DATA_REL		0xBB				//
#define ASM_CJNE_R2_DATA_REL		0xBA				//
#define ASM_CJNE_R1_DATA_REL		0xB9				//
#define ASM_CJNE_R0_DATA_REL		0xB8				//
#define ASM_CJNE_XR1_DATA_REL		0xB7				//
#define ASM_CJNE_XR0_DATA_REL		0xB6				//
#define ASM_CJNE_A_DIRECT_REL		0xB5				//
#define ASM_CJNE_A_DATA_REL			0xB4				//
#define ASM_CPL_C								0xB3				//翻转CY
#define ASM_CPL_Bit							0xB2				//翻转Bit
#define ASM_ANL_C_NBIT					0xB0				//
#define ASM_MOV_R7_DIRECT				0xAF				//
#define ASM_MOV_R6_DIRECT				0xAE				//
#define ASM_MOV_R5_DIRECT				0xAD				//
#define ASM_MOV_R4_DIRECT				0xAC				//
#define ASM_MOV_R3_DIRECT				0xAB				//
#define ASM_MOV_R2_DIRECT				0xAA				//
#define ASM_MOV_R1_DIRECT				0xA9				//
#define ASM_MOV_R0_DIRECT				0xA8				//
#define ASM_MOV_XR1_DIRECT			0xA7				//
#define ASM_MOV_XR0_DIRECT			0xA6				//
#define ASM_MUL_AB							0xA4				//乘法
#define ASM_INC_DPTR						0xA3				//
#define ASM_MOV_C_BIT						0xA2				//MOV C,Bit
#define ASM_ORL_C_NBIT					0xA0				//
#define ASM_SUBB_A_R7		  			0x9F				//
#define ASM_SUBB_A_R6		  			0x9E				//
#define ASM_SUBB_A_R5		  			0x9D				//
#define ASM_SUBB_A_R4		  			0x9C				//
#define ASM_SUBB_A_R3		  			0x9B				//
#define ASM_SUBB_A_R2		  			0x9A				//
#define ASM_SUBB_A_R1		  			0x99				//
#define ASM_SUBB_A_R0		  			0x98				//
#define ASM_SUBB_A_XR1		  		0x97				//
#define ASM_SUBB_A_XR0		  		0x96				//
#define ASM_SUBB_A_DIRECT				0x95				//
#define ASM_SUBB_A_DATA					0x94				//
#define ASM_MOVC_A_X_A_DPTR			0x93				//MOVC A,@A+DPTR
#define ASM_MOV_BIT_C						0x92				//MOV Bit,C
#define ASM_MOV_DPTR_DATA16			0x90				//立即数写入内存
#define ASM_MOV_DIRECT_R7				0x8F				//
#define ASM_MOV_DIRECT_R6				0x8E				//
#define ASM_MOV_DIRECT_R5				0x8D				//
#define ASM_MOV_DIRECT_R4				0x8C				//
#define ASM_MOV_DIRECT_R3				0x8B				//
#define ASM_MOV_DIRECT_R2				0x8A				//
#define ASM_MOV_DIRECT_R1				0x89				//
#define ASM_MOV_DIRECT_R0				0x88				//
#define ASM_MOV_DIRECT_XR1			0x87				//
#define ASM_MOV_DIRECT_XR0			0x86				//
#define ASM_MOV_DIRECT_DIRECT		0x85				//内存直接转移
#define ASM_DIV_AB							0x84				//除法
#define ASM_MOVC_A_X_A_PC				0x83				//MOVC A,@A+PC
#define ASM_ANL_C_BIT						0x82				//
#define ASM_SJMP_REL						0x80        //
#define ASM_MOV_R7_DATA					0x7F				//立即数写入R7
#define ASM_MOV_R6_DATA					0x7E				//立即数写入R6
#define ASM_MOV_R5_DATA					0x7D				//立即数写入R5
#define ASM_MOV_R4_DATA					0x7C				//立即数写入R4
#define ASM_MOV_R3_DATA					0x7B				//立即数写入R3
#define ASM_MOV_R2_DATA					0x7A				//立即数写入R2
#define ASM_MOV_R1_DATA					0x79				//立即数写入R1
#define ASM_MOV_R0_DATA					0x78				//立即数写入R0
#define ASM_MOV_XR1_DATA				0x77				//
#define ASM_MOV_XR0_DATA				0x76				//
#define ASM_MOV_DIRECT_DATA			0x75				//立即数写入内存
#define ASM_MOV_A_DATA					0x74				//立即数写入A
#define ASM_JMP_XA_DPTR					0x73				//
#define ASM_ORL_C_BIT						0x72				//
#define ASM_JNZ_REL							0x70				//
#define ASM_XRL_A_R7		  			0x6F				//
#define ASM_XRL_A_R6		  			0x6E				//
#define ASM_XRL_A_R5		  			0x6D				//
#define ASM_XRL_A_R4		  			0x6C				//
#define ASM_XRL_A_R3		  			0x6B				//
#define ASM_XRL_A_R2		  			0x6A				//
#define ASM_XRL_A_R1		  			0x69				//
#define ASM_XRL_A_R0		  			0x68				//
#define ASM_XRL_A_XR1		  			0x67				//
#define ASM_XRL_A_XR0		  			0x66				//
#define ASM_XRL_A_DIRECT				0x65				//
#define ASM_XRL_A_DATA					0x64				//
#define ASM_XRL_DIRECT_DATA			0x62				//
#define ASM_JZ_REL							0x60				//
#define ASM_ANL_A_R7		  			0x5F				//
#define ASM_ANL_A_R6		  			0x5E				//
#define ASM_ANL_A_R5		  			0x5D				//
#define ASM_ANL_A_R4		  			0x5C				//
#define ASM_ANL_A_R3		  			0x5B				//
#define ASM_ANL_A_R2		  			0x5A				//
#define ASM_ANL_A_R1		  			0x59				//
#define ASM_ANL_A_R0		  			0x58				//
#define ASM_ANL_A_XR1		  			0x57				//
#define ASM_ANL_A_XR0		  			0x56				//
#define ASM_ANL_A_DIRECT				0x55				//
#define ASM_ANL_A_DATA					0x54				//
#define ASM_ANL_DIRECT_DATA			0x52				//
#define ASM_JNC_REL			  			0x50				//
#define ASM_ORL_A_R7		  			0x4F				//
#define ASM_ORL_A_R6		  			0x4E				//
#define ASM_ORL_A_R5		  			0x4D				//
#define ASM_ORL_A_R4		  			0x4C				//
#define ASM_ORL_A_R3		  			0x4B				//
#define ASM_ORL_A_R2		  			0x4A				//
#define ASM_ORL_A_R1		  			0x49				//
#define ASM_ORL_A_R0		  			0x48				//
#define ASM_ORL_A_XR1		  			0x47				//
#define ASM_ORL_A_XR0		  			0x46				//
#define ASM_ORL_A_DIRECT				0x45				//
#define ASM_ORL_A_DATA					0x44				//
#define ASM_ORL_DIRECT_DATA			0x42				//
#define ASM_JC_REL			  			0x40				//
#define ASM_ADDC_A_R7		  			0x3F				//
#define ASM_ADDC_A_R6		  			0x3E				//
#define ASM_ADDC_A_R5		  			0x3D				//
#define ASM_ADDC_A_R4		  			0x3C				//
#define ASM_ADDC_A_R3		  			0x3B				//
#define ASM_ADDC_A_R2		  			0x3A				//
#define ASM_ADDC_A_R1		  			0x39				//
#define ASM_ADDC_A_R0		  			0x38				//
#define ASM_ADDC_A_XR1		  		0x37				//
#define ASM_ADDC_A_XR0		  		0x36				//
#define ASM_ADDC_A_DIRECT				0x35				//
#define ASM_ADDC_A_DATA					0x34				//
#define ASM_RLC_A								0x33				//
#define ASM_RETI								0x32				//
#define ASM_JNB_BIT_REL  				0x30				//
#define ASM_ADD_A_R7		  			0x2F				//
#define ASM_ADD_A_R6		  			0x2E				//
#define ASM_ADD_A_R5		  			0x2D				//
#define ASM_ADD_A_R4		  			0x2C				//
#define ASM_ADD_A_R3		  			0x2B				//
#define ASM_ADD_A_R2		  			0x2A				//
#define ASM_ADD_A_R1		  			0x29				//
#define ASM_ADD_A_R0		  			0x28				//
#define ASM_ADD_A_XR1		  			0x27				//
#define ASM_ADD_A_XR0		  			0x26				//
#define ASM_ADD_A_DIRECT				0x25				//
#define ASM_ADD_A_DATA					0x24				//
#define ASM_RL_A								0x23				//
#define ASM_RET									0x22				//
#define ASM_JB_BIT_REL  				0x20				//
#define ASM_DEC_R7		  				0x1F				//
#define ASM_DEC_R6		  				0x1E				//
#define ASM_DEC_R5		  				0x1D				//
#define ASM_DEC_R4		  				0x1C				//
#define ASM_DEC_R3		  				0x1B				//
#define ASM_DEC_R2		  				0x1A				//
#define ASM_DEC_R1		  				0x19				//
#define ASM_DEC_R0		  				0x18				//
#define ASM_DEC_XR1		  				0x17				//
#define ASM_DEC_XR0		  				0x16				//
#define ASM_DEC_DIRECT					0x15				//
#define ASM_DEC_A								0x14				//
#define ASM_RRC_A								0x13				//
#define ASM_LCALL_ADDR16	  		0x12				//
#define ASM_JBC_BIT_REL		  		0x10				//
#define ASM_INC_R7		  				0x0F				//
#define ASM_INC_R6		  				0x0E				//
#define ASM_INC_R5		  				0x0D				//
#define ASM_INC_R4		  				0x0C				//
#define ASM_INC_R3		  				0x0B				//
#define ASM_INC_R2		  				0x0A				//
#define ASM_INC_R1		  				0x09				//
#define ASM_INC_R0		  				0x08				//
#define ASM_INC_XR1		  				0x07				//
#define ASM_INC_XR0		  				0x06				//
#define ASM_INC_DIRECT					0x05				//
#define ASM_INC_A								0x04				//
#define ASM_RR_A								0x03				//
#define ASM_LJMP_ADDR16					0x02				//
#define ASM_NOP									0x00        //
#define ASM_ACALL_111_ADDR11		0xF1				//
#define ASM_ACALL_110_ADDR11		0xD1				//
#define ASM_ACALL_101_ADDR11		0xB1				//
#define ASM_ACALL_100_ADDR11		0x91				//
#define ASM_ACALL_011_ADDR11		0x71				//
#define ASM_ACALL_010_ADDR11		0x51				//
#define ASM_ACALL_001_ADDR11		0x31				//
#define ASM_ACALL_000_ADDR11		0x11				//
#define ASM_AJMP_111_ADDR11			0xE1				//
#define ASM_AJMP_110_ADDR11			0xC1				//
#define ASM_AJMP_101_ADDR11			0xA1				//
#define ASM_AJMP_100_ADDR11			0x81				//
#define ASM_AJMP_011_ADDR11			0x61				//
#define ASM_AJMP_010_ADDR11			0x41				//
#define ASM_AJMP_001_ADDR11			0x21				//
#define ASM_AJMP_000_ADDR11			0x01				//

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