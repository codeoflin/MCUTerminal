#include "Run51.h"

//程序计数器
unsigned int xdata PC=0;
//片内RAM直接寻址区 00~7F 虚拟寄存器 80~FF 有些物理寄存器无法映射给虚拟机用,此处虚拟一套给它们
unsigned char xdata MEM[0x100];
//片内RAM间接寻址区 00~FF 此区域共256字节,前面的128字节是MEM,后面128字节是独立的
unsigned char xdata IDATA[0x80];
//片外RAM扩展内存 这里给4KB吧
unsigned char xdata XDATA[SIZE_XDATA];
//虚拟机内FLASH起点,相对物理机FLASH偏移位置
long int xdata FlashOffset;
//程序状态字
unsigned char VPSW=0;
//VACC
unsigned char VACC=0;
//
unsigned char VSBUF=0;
//片内FLASH(MOVC指令访问用)
unsigned char CODE(unsigned int addr)
{
	return readFlash(FlashOffset+addr);
}

/* CPU微代码 根据PC寄存器读取1字节FLASH,然后PC寄存器+1 */
unsigned char readOne()
{
	unsigned char buff = CODE(PC);
	PC++;
	return buff;
}

/* CPU微代码 写字节寻址的MEM */
void writeByteDATA(unsigned char addr, unsigned char dat)
{
	if (addr < 0x80)
	{
		MEM[addr] = dat;
		return;
	}
	switch (addr)
	{
	case REG_P0:
		//sendStr("写入P0=");
		//sendUInt(dat);
		//sendLine(NULL);
		P0 = dat;
		break;
	case REG_P1:
		P1 = dat;
		break;
	case REG_P2:
		P2 = dat;
		break;
	case REG_P3:
		P3 = dat;
		break;
	case REG_PSW:
		VPSW = dat;
		break;
	case REG_ACC:
		VACC = dat;
		break;
	case REG_B:
	case REG_SP:
	case REG_DPH:
	case REG_DPL:
		MEM[addr] = dat;
		break;
	default:
		break;
	}
}

/* CPU微代码 读字节寻址的寄存器 */
unsigned char readByteDATA(unsigned char addr)
{
	if (addr < 0x80)return MEM[addr];
	switch (addr)
	{
	case REG_PSW:
		return VPSW;
	case REG_ACC:
		return VACC;
	case REG_P0:
		return P0;
	case REG_P1:
		return P1;
	case REG_P2:
		return P2;
	case REG_P3:
		return P3;
	//有些寄存器 无法映射,直接取虚拟内存
	case REG_B:
	case REG_SP:
	case REG_DPH:
	case REG_DPL:
		return MEM[addr];
	default:
		return 0;
	}
}

/* CPU微代码 写字节寻址的MEM */
void writeByteIDATA(unsigned char addr, unsigned char dat)
{
	if (addr < 0x80)
	{
		MEM[addr] = dat;
		return;
	}
	IDATA[addr - 0x80] = dat;
}
/* CPU微代码 读字节寻址的寄存器 */
unsigned char readByteIDATA(unsigned char addr)
{
	if (addr < 0x80)return MEM[addr];
	return IDATA[addr - 0x80];
}

/* CPU微代码 读R系列寄存器 */
unsigned char readREG_Rx(unsigned char mcode)
{
	mcode = mcode & 0xF;
	if (mcode < 8)mcode = mcode - 6;
	return readByteDATA((VPSW & 0x18) + (mcode & 7));
}

/* CPU微代码 写R系列寄存器 */
void writeREG_Rx(unsigned char mcode, unsigned char dat)
{
	mcode = mcode & 0xF;
	if (mcode < 8)mcode = mcode - 6;
	writeByteDATA((VPSW & 0x18) + (mcode & 7), dat);
}

/* CPU微代码 立即数入栈 */
void push(unsigned char dat)
{
	unsigned char sp = readByteDATA(REG_SP);
	sp += 1;
	writeByteIDATA(sp, dat);//可以写入片内 高128字节
	writeByteDATA(REG_SP, sp);
}

/* CPU微代码 出栈到指定地址(支持出栈到IDATA) */
void pop(unsigned char addr)
{
	unsigned char dat;
	unsigned char sp = readByteDATA(REG_SP);
	dat = readByteIDATA(sp);
	sp -= 1;
	writeByteDATA(REG_SP, sp);
	writeByteDATA(addr, dat);
}

/* CPU微代码 位操作_写*/
void writeBit(unsigned char addr, unsigned char dat)
{
	unsigned char temp = 0;
	unsigned char addr2 = addr < 0x80 ? ((addr >> 4) * 2) + 0x20 : addr & 0xF0;
	temp = readByteDATA(addr2);
	temp = dat ? temp | 1 << (addr & 0xF) : temp & (0xFF - (1 << (addr & 0xF)));
	writeByteDATA(addr2, temp);
}

/* CPU微代码 位操作_读*/
unsigned char readBit(unsigned char addr)
{
	return readByteDATA(addr < 0x80 ? ((addr >> 4) * 2) + 0x20 : addr & 0xF0);
}

/* CPU微代码 累加器加运算 */
void addToACC(unsigned char dat, unsigned char addcy)
{
	unsigned char oldcy = VPSW & 0x80;
	unsigned char acc = VACC;
	VPSW &= 0x7F & 0xBF & 0xFD & 0xFE;
	if (addcy && oldcy)
	{
		VACC += 1;
		if (VACC < acc)VPSW |= 0x80 | 0x02;//CY,进位,OV带符号进位
		if ((VACC & 0xF) == 0) VPSW |= 0x40;//AC,低4位进位
		if (((VACC & 0x7F) == 0) ^ (VACC < acc)) VPSW |= 0x02;//OV,OV带符号进位
	}
	VACC += dat;
	if (VACC < acc)VPSW |= 0x80 | 0x02;//CY,进位,OV带符号进位
	if ((VACC & 0xF) < (acc & 0xF)) VPSW |= 0x40;//AC,低4位进位
	if (((VACC & 0x7F) < (acc & 0x7F)) ^ (VACC < acc)) VPSW |= 0x02;//OV,OV带符号进位
}

/* CPU微代码 累加器减运算 */
void subFromACC(unsigned char dat, unsigned char subcy)
{
	unsigned char oldcy = VPSW & 0x80;
	unsigned char acc = VACC;
	VPSW &= 0x7F & 0xBF & 0xFD & 0xFE;
	if (subcy && oldcy)
	{
		VACC--;
		if (VACC > acc)VPSW |= 0x80 | 0x02;//CY,借位
		if ((VACC & 0xF) == 0xF) VPSW |= 0x40;//AC,低4位进位
		if (((VACC & 0x7F) == 0xF) ^ (VACC > acc)) VPSW |= 0x02;//OV,OV带符号进位
		acc = VACC;
	}
	VACC -= dat;
	if (VACC > acc)VPSW |= 0x80;//CY,借位
	if ((VACC & 0xF) > (acc & 0xF)) VPSW |= 0x40;//AC,低4位借位
	if (((VACC & 0x7F) > (acc & 0x7F)) ^ (VACC > acc)) VPSW |= 0x02;//OV,OV带符号借位
}

/* 运行 */
void run(long int addr,long int len)
{
	//机器码
	MACHINECODE mcode = 0;
	unsigned char direct1 = 0;
	unsigned char direct2 = 0;
	char dat1 = 0;
	char dat2 = 0;
	unsigned char udat1 = 0;
	unsigned char udat2 = 0;
	unsigned char udat3 = 0;
	unsigned char udat4 = 0;
	unsigned char udat5 = 0;
	unsigned char udat6 = 0;
	unsigned char bit1 = 0;
	char flag_run = 1;
	unsigned int uitemp = 0;
	long int i=0;
	FlashOffset = addr;
	//初始化寄存器
	PC = 0;
	VACC = 0;
	for(i=0;i<0x100;i++)MEM[i]=0;
	for(i=0;i<SIZE_XDATA;i++)XDATA[i]=0;
	writeByteDATA(REG_B, 0);
	writeByteDATA(REG_DPL, 0);
	writeByteDATA(REG_DPH, 0);
	writeByteDATA(REG_SP, 7);//堆栈指针初始化

	while (flag_run)
	{
		mcode = readOne();
		//这几行是为了判断A寄存器里1的数量是单还是双
		udat1 = VACC;
		VPSW &= 0xFE;
		for (; udat1 > 0; udat1 = udat1 >> 1)if (udat1 & 1)VPSW ^= 1;
		if(RI!=0)
		{
			VSBUF=SBUF;
			RI=0;
			if(VSBUF=='c'||VSBUF=='C')
			{
				sendLine("强制退出!");
				flag_run=0;
			}
		}
		/*
		if(1!=1
		)
		// */
		{
			sendStr("mcode=");
			sendHexByte(mcode);
			sendStr(" ACC=");
			sendHexByte(VACC);
			sendStr(" PSW=");
			sendHexByte(VPSW);
			sendStr(" R0~7=");
			sendHexByte(readREG_Rx(8));
			sendStr(" ");
			sendHexByte(readREG_Rx(9));
			sendStr(" ");
			sendHexByte(readREG_Rx(10));
			sendStr(" ");
			sendHexByte(readREG_Rx(11));
			sendStr(" ");
			sendHexByte(readREG_Rx(12));
			sendStr(" ");
			sendHexByte(readREG_Rx(13));
			sendStr(" ");
			sendHexByte(readREG_Rx(14));
			sendStr(" ");
			sendHexByte(readREG_Rx(15));
			sendStr(" SP=");
			sendHexByte(readByteDATA(REG_SP));
			sendStr(" P0=");
			sendHexByte(readByteDATA(REG_P0));
			sendStr(" P1=");
			sendHexByte(readByteDATA(REG_P1));
			sendStr(" P2=");
			sendHexByte(readByteDATA(REG_P2));
			sendStr(" P3=");
			sendHexByte(readByteDATA(REG_P3));

			sendStr(" DPTR=");
			sendHexByte(readByteDATA(REG_DPH));
			sendHexByte(readByteDATA(REG_DPL));
			sendLine(NULL);
			//for (i = 0x4FFF; i>0; i--)i+=0;
		}
		switch (mcode)
		{
		case ASM_MOV_R7_A://FF
		case ASM_MOV_R6_A://FE
		case ASM_MOV_R5_A://FD
		case ASM_MOV_R4_A://FC
		case ASM_MOV_R3_A://FB
		case ASM_MOV_R2_A://FA
		case ASM_MOV_R1_A://F9
		case ASM_MOV_R0_A://F8
			writeREG_Rx(mcode, VACC);
			break;
		case ASM_MOV_XR1_A://F7
		case ASM_MOV_XR0_A://F6
			writeByteIDATA(readREG_Rx(mcode), VACC);
			break;
		case ASM_MOV_DIRECT_A://F5
			direct1 = readOne();
			writeByteDATA(direct1, VACC);
			break;
		case ASM_CPL_A://F4
			VACC = VACC ^ 0xFF;
			break;
		case ASM_MOVX_XR1_A://F3
		case ASM_MOVX_XR0_A://F2
			XDATA[readREG_Rx(mcode)] = VACC;
			break;
		case ASM_MOVX_XDPTR_A://F0 X
			uitemp = readByteDATA(REG_DPH) << 8;
			uitemp = uitemp + readByteDATA(REG_DPL);
			XDATA[uitemp] = VACC;
			break;
		case ASM_MOV_A_R7://EF
		case ASM_MOV_A_R6://EE
		case ASM_MOV_A_R5://ED
		case ASM_MOV_A_R4://EC
		case ASM_MOV_A_R3://EB
		case ASM_MOV_A_R2://EA
		case ASM_MOV_A_R1://E9
		case ASM_MOV_A_R0://E8
			VACC = readREG_Rx(mcode);
			break;
		case ASM_MOV_A_XR1://E7 X
		case ASM_MOV_A_XR0://E6 X
			VACC = readByteIDATA(readREG_Rx(mcode));
			break;
		case ASM_MOV_A_DIRECT://E5
			direct1 = readOne();
			VACC = readByteDATA(direct1);
			break;
		case ASM_CLR_A://E4
			VACC = 0;
			break;
		case ASM_MOVX_A_XR1://E3
		case ASM_MOVX_A_XR0://E2
			VACC = XDATA[readREG_Rx(mcode)];
			break;
		case ASM_MOVX_A_XDPTR://E0
			uitemp = readByteDATA(REG_DPH) << 8;
			uitemp = uitemp + readByteDATA(REG_DPL);
			VACC = XDATA[uitemp];
			break;
		case ASM_DJNZ_R7_REL://DF
		case ASM_DJNZ_R6_REL://DE
		case ASM_DJNZ_R5_REL://DD
		case ASM_DJNZ_R4_REL://DC
		case ASM_DJNZ_R3_REL://DB
		case ASM_DJNZ_R2_REL://DA
		case ASM_DJNZ_R1_REL://D9
		case ASM_DJNZ_R0_REL://D8
			udat1 = readREG_Rx(mcode) - 1;
			dat1 = readOne();
			writeREG_Rx(mcode, udat1);
			if (udat1 == 0)break;
			PC += dat1;
			break;
		case ASM_XCHD_A_XR1://D7
		case ASM_XCHD_A_XR0://D6
			udat1 = VACC;
			udat2 = readByteIDATA(readREG_Rx(mcode));
			VACC = (udat1 & 0xF0) + (udat2 & 0xF);
			writeByteIDATA(readREG_Rx(mcode), (udat2 & 0xF0) + (udat1 & 0xF));
			break;
		case ASM_DJNZ_DIRECT_REL://D5
			direct1 = readOne();
			dat1 = readOne();
			udat1 = readByteDATA(direct1);
			writeByteDATA(direct1, udat1 - 1);
			if (udat1 == 0)break;
			PC += dat1;
			break;
		case ASM_DA_A://D4 X 十进制加后整理
			//首先把数字分为个位和十位
			udat2 = VACC & 0xF;//临时个位
			udat3 = VACC >> 4;//临时十位

			//如果个位曾经进位过,则个位+6,十位类推
			if (VPSW & 0x40)udat2 += 6;
			if (VPSW & 0x80)udat3 += 6;

			//整理个位
			if (udat2 > 9)
			{
				udat3 += udat2 / 10;
				udat2 = udat2 % 10;
			}

			//整理十位
			if (udat3 > 9)
			{
				VPSW |= 0x80;
				udat3 = udat3 % 10;
			}

			//合并 送回VACC
			VACC = (udat3 << 4) + udat2;
			break;
		case ASM_SETB_C://D3
			writeBit(BIT_CY, 1);
			break;
		case ASM_SETB_BIT://D2
			bit1 = readOne();
			writeBit(bit1, 1);
			break;
		case ASM_POP_DIRECT://D0
			direct1 = readOne();
			pop(direct1);
			break;
		case ASM_XCH_A_R7://CF
		case ASM_XCH_A_R6://CE
		case ASM_XCH_A_R5://CD
		case ASM_XCH_A_R4://CC
		case ASM_XCH_A_R3://CB
		case ASM_XCH_A_R2://CA
		case ASM_XCH_A_R1://C9
		case ASM_XCH_A_R0://C8
			udat1 = VACC;
			VACC = readREG_Rx(mcode);
			writeREG_Rx(mcode, udat1);
			break;
		case ASM_XCH_A_XR1://C7
		case ASM_XCH_A_XR0://C6
			udat1 = VACC;
			VACC = readByteIDATA(readREG_Rx(mcode));
			writeByteIDATA(readREG_Rx(mcode), udat1);
			break;
		case ASM_XCH_A_DIRECT://C5
			udat1 = VACC;
			direct1 = readOne();
			VACC = readByteDATA(direct1);
			writeByteDATA(direct1, udat1);
			break;
		case ASM_SWAP_A://C4 X
			VACC = ((VACC & 0xF) >> 4) + ((VACC & 0xF0) << 4);
			break;
		case ASM_CLR_C://C3
			writeBit(BIT_CY, 0);
			break;
		case ASM_CLR_BIT://C2
			bit1 = readOne();
			writeBit(bit1, 0);
			break;
		case ASM_PUSH_DIRECT://C0
			direct1 = readOne();
			push(readByteDATA(direct1));
			break;
		case ASM_CPL_C://B3 X
			VPSW ^= 0x80;
			break;
		case ASM_CPL_Bit://B2 X
			bit1 = readOne();
			writeBit(bit1, readBit(bit1) == 0);
			break;
		case ASM_ANL_C_NBIT://B0 X
			bit1 = readOne();
			VPSW = (((VPSW & 0x80) && (bit1 == 0)) * 0x80 + (VPSW & 0x7F));
			break;
		case ASM_MOV_R7_DIRECT://AF
		case ASM_MOV_R6_DIRECT://AE
		case ASM_MOV_R5_DIRECT://AD
		case ASM_MOV_R4_DIRECT://AC
		case ASM_MOV_R3_DIRECT://AB
		case ASM_MOV_R2_DIRECT://AA
		case ASM_MOV_R1_DIRECT://A9
		case ASM_MOV_R0_DIRECT://A8
			direct1 = readOne();
			writeREG_Rx(mcode, readByteDATA(direct1));
			break;
		case ASM_MOV_XR1_DIRECT://A7 X
		case ASM_MOV_XR0_DIRECT://A6 X
			direct1 = readOne();
			writeByteIDATA(readREG_Rx(mcode), readByteDATA(direct1));
			break;
		case ASM_MUL_AB://A4 X
			udat2 = readByteDATA(REG_B);
			uitemp = (unsigned int)VACC * (unsigned int)udat2;
			VACC = uitemp;
			writeByteDATA(REG_B, uitemp >> 8);
			break;
		case ASM_INC_DPTR://A3
			uitemp = readByteDATA(REG_DPH) << 8;
			uitemp += readByteDATA(REG_DPL);
			uitemp += 1;
			writeByteDATA(REG_DPL, uitemp);
			writeByteDATA(REG_DPH, uitemp >> 8);
			break;
		case ASM_MOV_C_BIT://A2
			bit1 = readOne();
			writeBit(BIT_CY, readBit(bit1));
			break;
		case ASM_ORL_C_NBIT://A0 X
			bit1 = readOne();
			VPSW = (((VPSW & 0x80) || bit1) * 0x80 + (VPSW & 0x7F));
			break;
		case ASM_SUBB_A_R7://9F X
		case ASM_SUBB_A_R6://9E X
		case ASM_SUBB_A_R5://9D X
		case ASM_SUBB_A_R4://9C X
		case ASM_SUBB_A_R3://9B X
		case ASM_SUBB_A_R2://9A X
		case ASM_SUBB_A_R1://99 X
		case ASM_SUBB_A_R0://98 X
			udat1 = readREG_Rx(mcode);
			subFromACC(udat1, 1);
			break;
		case ASM_SUBB_A_XR1://97 X
		case ASM_SUBB_A_XR0://96 X
			direct1 = readREG_Rx(mcode);
			udat1 = readByteDATA(direct1);
			subFromACC(udat1, 1);
			break;
		case ASM_SUBB_A_DIRECT://95 X
			direct1 = readOne();
			udat1 = readByteDATA(direct1);
			subFromACC(udat1, 1);
			break;
		case ASM_SUBB_A_DATA://94 X
			udat1 = readOne();
			subFromACC(udat1, 1);
			break;
		case ASM_MOVC_A_X_A_DPTR://93
			uitemp = readByteDATA(REG_DPH) << 8;
			uitemp = uitemp + readByteDATA(REG_DPL);
			VACC = CODE(uitemp + VACC);
			break;
		case ASM_MOV_BIT_C://92
			bit1 = readOne();
			writeBit(bit1, readBit(BIT_CY));
			break;
		case ASM_MOV_DPTR_DATA16://90
			udat1 = readOne();
			udat2 = readOne();
			writeByteDATA(REG_DPH, udat1);
			writeByteDATA(REG_DPL, udat2);
			break;
		case ASM_MOV_DIRECT_R7://8F
		case ASM_MOV_DIRECT_R6://8E
		case ASM_MOV_DIRECT_R5://8D
		case ASM_MOV_DIRECT_R4://8C
		case ASM_MOV_DIRECT_R3://8B
		case ASM_MOV_DIRECT_R2://8A
		case ASM_MOV_DIRECT_R1://89
		case ASM_MOV_DIRECT_R0://88
			direct1 = readOne();
			writeByteDATA(direct1, readREG_Rx(mcode));
			break;
		case ASM_MOV_DIRECT_XR1://87 X
		case ASM_MOV_DIRECT_XR0://86 X
			direct1 = readOne();
			udat1 = readByteIDATA(readREG_Rx(mcode));
			writeByteDATA(direct1, udat1);
			break;
		case ASM_MOV_DIRECT_DIRECT://85
			direct1 = readOne();
			direct2 = readOne();
			writeByteDATA(direct2, readByteDATA(direct1));
			break;
		case ASM_DIV_AB://84 X
			udat1 = VACC;
			udat2 = readByteDATA(REG_B);
			VACC = udat1 / udat2;
			writeByteDATA(REG_B, udat1 % udat2);
			break;
		case ASM_CJNE_R7_DATA_REL://8F X
		case ASM_CJNE_R6_DATA_REL://8E X
		case ASM_CJNE_R5_DATA_REL://8D X
		case ASM_CJNE_R4_DATA_REL://8C X
		case ASM_CJNE_R3_DATA_REL://8B X
		case ASM_CJNE_R2_DATA_REL://8A X
		case ASM_CJNE_R1_DATA_REL://89 X
		case ASM_CJNE_R0_DATA_REL://88 X
			udat1 = readOne();
			dat1 = readOne();
			if (readREG_Rx(mcode) == udat1)break;
			PC += dat1;
			break;
		case ASM_CJNE_XR1_DATA_REL://87 X
		case ASM_CJNE_XR0_DATA_REL://86 X
			udat1 = readOne();
			dat1 = readOne();
			if (readByteIDATA(readREG_Rx(mcode)) == udat1)break;
			PC += dat1;
			break;
		case ASM_CJNE_A_DIRECT_REL://85 X
			udat1 = readByteDATA(readOne());
			dat1 = readOne();
			if (VACC == udat1)break;
			PC += dat1;
			break;
		case ASM_CJNE_A_DATA_REL://84 X
			udat1 = readOne();
			dat1 = readOne();
			if (VACC == udat1)break;
			PC += dat1;
			break;
		case ASM_MOVC_A_X_A_PC://83
			udat1 = CODE(PC + VACC);
			VACC = udat1;
			break;
		case ASM_ANL_C_BIT://82 X
			bit1 = readOne();
			VPSW = (((VPSW & 0x80) && bit1) * 0x80 + (VPSW & 0x7F));
			break;
		case ASM_SJMP_REL://80
			dat1 = readOne();
			PC += dat1;
			break;
		case ASM_MOV_R7_DATA://7F
		case ASM_MOV_R6_DATA://7E
		case ASM_MOV_R5_DATA://7D
		case ASM_MOV_R4_DATA://7C
		case ASM_MOV_R3_DATA://7B
		case ASM_MOV_R2_DATA://7A
		case ASM_MOV_R1_DATA://79
		case ASM_MOV_R0_DATA://78
			udat1 = readOne();
			writeREG_Rx(mcode, udat1);
			break;
		case ASM_MOV_XR1_DATA://77
		case ASM_MOV_XR0_DATA://76
			udat1 = readOne();
			writeByteIDATA(readREG_Rx(mcode), udat1);
			break;
		case ASM_MOV_DIRECT_DATA://75
			direct1 = readOne();
			udat1 = readOne();
			writeByteDATA(direct1, udat1);
			break;
		case ASM_MOV_A_DATA://74
			udat1 = readOne();
			VACC = udat1;
			break;
		case ASM_JMP_XA_DPTR://73 X
			uitemp = readByteDATA(REG_DPH) << 8;
			uitemp = uitemp + readByteDATA(REG_DPL);
			PC = VACC + uitemp;
			break;
		case ASM_ORL_C_BIT://72 X
			bit1 = readOne();
			VPSW = (((VPSW & 0x80) || bit1) * 0x80 + (VPSW & 0x7F));
			break;
		case ASM_XRL_A_R7://6F X
		case ASM_XRL_A_R6://6E X
		case ASM_XRL_A_R5://6D X
		case ASM_XRL_A_R4://6C X
		case ASM_XRL_A_R3://6B X
		case ASM_XRL_A_R2://6A X
		case ASM_XRL_A_R1://69 X
		case ASM_XRL_A_R0://68 X
			udat1 = readREG_Rx(mcode);
			VACC = udat1 ^ VACC;
			break;
		case ASM_XRL_A_XR1://67 X
		case ASM_XRL_A_XR0://66 X
			direct1 = readREG_Rx(mcode);
			udat1 = readByteDATA(direct1);
			VACC ^= udat1;
			break;
		case ASM_XRL_A_DIRECT://65 X
			direct1 = readOne();
			udat1 = readByteDATA(direct1);
			VACC ^= udat1;
			break;
		case ASM_XRL_A_DATA://64 X
			udat1 = readOne();
			VACC ^= udat1;
			break;
		case ASM_XRL_DIRECT_DATA://63 X
			direct1 = readOne();
			udat1 = readByteDATA(direct1);
			udat2 = readOne();
			writeByteDATA(direct1, udat1 ^ udat2);
			break;
		case ASM_JNZ_REL://61
			dat1 = readOne();
			if (!VACC) break;
			PC += dat1;
			break;
		case ASM_JZ_REL://60
			dat1 = readOne();
			if (VACC) break;
			PC += dat1;
			break;
		case ASM_ANL_A_R7://5F X
		case ASM_ANL_A_R6://5E X
		case ASM_ANL_A_R5://5D X
		case ASM_ANL_A_R4://5C X
		case ASM_ANL_A_R3://5B X
		case ASM_ANL_A_R2://5A X
		case ASM_ANL_A_R1://59 X
		case ASM_ANL_A_R0://58 X
			udat1 = readREG_Rx(mcode);
			VACC &= udat1;
			break;
		case ASM_ANL_A_XR1://57 X
		case ASM_ANL_A_XR0://56 X
			direct1 = readREG_Rx(mcode);
			udat1 = readByteDATA(direct1);
			VACC &= udat1;
			break;
		case ASM_ANL_A_DIRECT://55 X
			direct1 = readOne();
			udat1 = readByteDATA(direct1);
			VACC &= udat1;
			break;
		case ASM_ANL_A_DATA://54 X
			udat1 = readOne();
			VACC &= udat1;
			break;
		case ASM_ANL_DIRECT_DATA://53 X
			direct1 = readOne();
			udat1 = readByteDATA(direct1);
			udat2 = readOne();
			writeByteDATA(direct1, udat1 & udat2);
			break;
		case ASM_JNC_REL://50 X
			dat1 = readOne();
			if (VPSW & 0x80) break;
			PC += dat1;
			break;
		case ASM_ORL_A_R7://4F X
		case ASM_ORL_A_R6://4E X
		case ASM_ORL_A_R5://4D X
		case ASM_ORL_A_R4://4C X
		case ASM_ORL_A_R3://4B X
		case ASM_ORL_A_R2://4A X
		case ASM_ORL_A_R1://49 X
		case ASM_ORL_A_R0://48 X
			udat1 = readREG_Rx(mcode);
			VACC |= udat1;
			break;
		case ASM_ORL_A_XR1://47 X
		case ASM_ORL_A_XR0://46 X
			direct1 = readREG_Rx(mcode);
			udat1 = readByteDATA(direct1);
			VACC |= udat1;
			break;
		case ASM_ORL_A_DIRECT://45 X
			direct1 = readOne();
			udat1 = readByteDATA(direct1);
			VACC |= udat1;
			break;
		case ASM_ORL_A_DATA://44 X
			udat1 = readOne();
			VACC |= udat1;
			break;
		case ASM_ORL_DIRECT_DATA://43
			direct1 = readOne();
			udat1 = readByteDATA(direct1);
			udat2 = readOne();
			writeByteDATA(direct1, udat1 | udat2);
			break;
		case ASM_ORL_DIRECT_A://42
			direct1 = readOne();
			udat1 = readByteDATA(direct1);
			writeByteDATA(direct1, udat1 | VACC);
			break;
		case ASM_JC_REL://40
			dat1 = readOne();
			if (!(VPSW & 0x80)) break;
			PC += dat1;
			break;
		case ASM_ADDC_A_R7://3F X
		case ASM_ADDC_A_R6://3E X
		case ASM_ADDC_A_R5://3D X
		case ASM_ADDC_A_R4://3C X
		case ASM_ADDC_A_R3://3B X
		case ASM_ADDC_A_R2://3A X
		case ASM_ADDC_A_R1://39 X
		case ASM_ADDC_A_R0://38 X
			udat1 = readREG_Rx(mcode);
			addToACC(udat1, 1);
			break;
		case ASM_ADDC_A_XR1://37 X
		case ASM_ADDC_A_XR0://36 X
			direct1 = readREG_Rx(mcode);
			udat1 = readByteDATA(direct1);
			addToACC(udat1, 1);
			break;
		case ASM_ADDC_A_DIRECT://35
			direct1 = readOne();
			udat1 = readByteDATA(direct1);
			addToACC(udat1, 1);
			break;
		case ASM_ADDC_A_DATA://34
			udat1 = readOne();
			addToACC(udat1, 1);
			break;
		case ASM_RLC_A://33 X
			udat1 = VACC << 1;
			if (VPSW & 0x80)udat1++;
			VPSW &= 0x7F;
			if (VACC >= 0x80)VPSW |= 0x80;
			VACC = udat1;
			break;
		case ASM_RETI://31
			direct1 = readByteDATA(REG_SP);
			writeByteDATA(REG_SP, direct1 - 2);
			PC = (MEM[direct1] << 8) + MEM[direct1 - 1];
			break;
		case ASM_JNB_BIT_REL://30 X
			direct1 = readOne();
			bit1 = readBit(direct1);
			if (bit1)break;
			dat1 = readOne();
			PC += dat1;
			break;
		case ASM_ADD_A_R7://2F X
		case ASM_ADD_A_R6://2E X
		case ASM_ADD_A_R5://2D X
		case ASM_ADD_A_R4://2C X
		case ASM_ADD_A_R3://2B X
		case ASM_ADD_A_R2://2A X
		case ASM_ADD_A_R1://29 X
		case ASM_ADD_A_R0://28 X
			udat1 = readREG_Rx(mcode);
			addToACC(udat1, 0);
			break;
		case ASM_ADD_A_XR1://27 X
		case ASM_ADD_A_XR0://26 X
			direct1 = readREG_Rx(mcode);
			udat1 = readByteDATA(direct1);
			addToACC(udat1, 0);
			break;
		case ASM_ADD_A_DIRECT://25
			direct1 = readOne();
			udat1 = readByteDATA(direct1);
			addToACC(udat1, 0);
			break;
		case ASM_ADD_A_DATA://24
			udat1 = readOne();
			addToACC(udat1, 0);
			break;
		case ASM_RL_A://23
			udat1=VACC<<1;
			if(VACC>=0x80)udat1++;
			VACC=udat1;
			break;
		case ASM_RET://22
			direct1 = readByteDATA(REG_SP);
			writeByteDATA(REG_SP, direct1 - 2);
			PC = (MEM[direct1] << 8) + MEM[direct1 - 1];
			break;
		case ASM_JB_BIT_REL://20 X
			direct1 = readOne();
			dat1 = readOne();
			bit1 = readBit(direct1);
			if (!bit1)break;
			PC += dat1;
			break;
		case ASM_DEC_R7://1F X
		case ASM_DEC_R6://1E X
		case ASM_DEC_R5://1D X
		case ASM_DEC_R4://1C X
		case ASM_DEC_R3://1B X
		case ASM_DEC_R2://1A X
		case ASM_DEC_R1://19 X
		case ASM_DEC_R0://18 X
			udat1 = readREG_Rx(mcode);
			writeREG_Rx(mcode, udat1 - 1);
			break;
		case ASM_DEC_XR1://17 X
		case ASM_DEC_XR0://16 X
			direct1 = readREG_Rx(mcode);
			udat1 = readByteIDATA(direct1);
			writeByteIDATA(direct1, udat1 - 1);
			break;
		case ASM_DEC_DIRECT://15
			direct1 = readOne();
			udat1 = readByteDATA(direct1);
			writeByteDATA(direct1, udat1 - 1);
			break;
		case ASM_DEC_A://14
			VACC--;
			break;
		case ASM_RRC_A://13 X
			udat1 = VACC >> 1;
			if (VPSW & 0x80)udat1 += 0x80;
			VPSW &= 0x7f;
			if (VACC >= 0x80)VPSW |= 0x80;
			VACC = udat1;
			break;
		case ASM_LCALL_ADDR16://12
			direct1 = readOne();
			direct2 = readOne();
			push(PC);
			push(PC >> 8);
			PC = (direct1 << 8) + direct2;
			break;
		case ASM_JBC_BIT_REL://10 X
			direct1 = readOne();
			dat1 = readOne();
			bit1 = readBit(direct1);
			if (!bit1)break;
			writeBit(direct1, 0);
			PC += dat1;
			break;
		case ASM_INC_R7://0F X
		case ASM_INC_R6://0E X
		case ASM_INC_R5://0D X
		case ASM_INC_R4://0C X
		case ASM_INC_R3://0B X
		case ASM_INC_R2://0A X
		case ASM_INC_R1://09 X
		case ASM_INC_R0://08 X
			udat1 = readREG_Rx(mcode);
			writeREG_Rx(mcode, udat1 + 1);
			break;
		case ASM_INC_XR1://07 X
		case ASM_INC_XR0://06 X
			direct1 = readREG_Rx(mcode);
			udat1 = readByteIDATA(direct1);
			writeByteIDATA(direct1, udat1 + 1);
			break;
		case ASM_INC_DIRECT://05
			direct1 = readOne();
			udat1 = readByteDATA(direct1);
			writeByteDATA(direct1, udat1 + 1);
			break;
		case ASM_INC_A://04
			VACC++;
			break;
		case ASM_RR_A://03
			udat1=VACC>>1;
			if(VACC&1)udat1+=0x80;
			VACC=udat1;
			break;
		case ASM_LJMP_ADDR16://02
			direct1 = readOne();
			direct2 = readOne();
			PC = (direct1 << 8) + direct2;
			break;
		case ASM_NOP://00
			if(readREG_Rx(4)!='L')break;
			if(readREG_Rx(5)!='i')break;
			if(readREG_Rx(6)!='n')break;
			if(readREG_Rx(7)!='z')break;
			//如果R4~R7的内容为'linz',则进入特殊模式
			if(ACC==1)
			{
				sendLine("程序退出!");
				sendLine(NULL);
				flag_run=0;
			}
			break;
		case ASM_AJMP_111_ADDR11://E1
		case ASM_AJMP_110_ADDR11://C1
		case ASM_AJMP_101_ADDR11://A1
		case ASM_AJMP_100_ADDR11://81
		case ASM_AJMP_011_ADDR11://61
		case ASM_AJMP_010_ADDR11://41
		case ASM_AJMP_001_ADDR11://21
		case ASM_AJMP_000_ADDR11://01
			direct1 = readOne();
			PC = ((0xF800 & PC) + ((mcode & 0xE0) << 3) + direct1);
			break;

		case ASM_ACALL_111_ADDR11://F1
		case ASM_ACALL_110_ADDR11://D1
		case ASM_ACALL_101_ADDR11://B1
		case ASM_ACALL_100_ADDR11://91
		case ASM_ACALL_011_ADDR11://71
		case ASM_ACALL_010_ADDR11://51
		case ASM_ACALL_001_ADDR11://31
		case ASM_ACALL_000_ADDR11://11
			direct1 = readOne();
			push(PC);
			push(PC >> 8);
			PC = ((0xF800 & PC) + ((mcode & 0xE0) << 3) + direct1);
			break;
		default:
			sendLine("不认识的指令!");
			sendUInt(mcode);
			sendLine(NULL);
			flag_run = 0;
			break;
		}//End Switch
    }//End While
}

void run51()
{
	char argc=Argc;
	const char **argv=Argv;
	//起点地址,要读写的长度
	long int addr=0,len=0;
	int i=0;
	//输入命令有错误的时候用这个提示用户输入正确的命令
	char* example="run51 0x8000 0x100\r\n     run51 0x8000";
	switch(argc)
	{
	case 2:
		addr=toLong(argv[1]);//解析地址
		len=0x100;
		run(addr,len);
		break;
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