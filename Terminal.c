#include "Terminal.h"
#include "Commands.h"
/*****************************************************************************/
char idata SerialBuffer[MAX_SERIAL_BUFFER_SIZE];
char idata VTCmdBuffer[MAX_VT_COMMAND_BUFFER_SIZE];//
char code PromptBuffer[]=">";
char idata CursorPosion;/* 命令行输入缓冲区指针 */
char idata VTCursorPosion;/* VT命令缓冲区指针 */
char idata ExecCommandFlag;/* 执行命令标志 */
char idata VTControlModeFlag;/* VT控制模式 */
/*****************************************************************************/
/* 开机初始化 */
void InitTerminal(void)
{
	CursorPosion = 0;
	ExecCommandFlag = 0;
	VTControlModeFlag = 0;
	SendStr(DEFAULT_F_COLOR);
	SendStr(DEFAULT_B_COLOR);
	SendStr(CLEARSCREEN);
	SendStr(CURSORHOME);
	memset(SerialBuffer,'\0',MAX_SERIAL_BUFFER_SIZE);
	memset(VTCmdBuffer,'\0',MAX_VT_COMMAND_BUFFER_SIZE);
	SendLine("*****************************");
	SendLine2("    Linz Terminal System    ",F_RED,DEFAULT_B_COLOR);
	SendLine2("        林子终端系统         ",F_BLUE,DEFAULT_B_COLOR);
	SendLine("*****************************");
	SendStr("\r\n");
	SendStr2(PromptBuffer,PROMPT_F_COLOR,DEFAULT_B_COLOR);
}

/* VT输入 */
void VTInput(unsigned char sbuftemp)
{
	int argv[]={0,0,0,0,0,0,0,0};//其实vt参数最多3个
	int argc=0;
	int i;
	//在VT里面英文字母表示结束,0x24 + "[" 表示开始
	switch(sbuftemp)
	{
	case 'a':
	case 'b':
	case 'c':
	case 'd':
	case 'e':
	case 'f':
	case 'g':
	case 'h':
	case 'i':
	case 'j':
	case 'k':
	case 'l':
	case 'm':
	case 'n':
	case 'o':
	case 'p':
	case 'q':
	case 'r':
	case 's':
	case 't':
	case 'u':
	case 'v':
	case 'w':
	case 'x':
	case 'y':
	case 'z':
	case 'A':
	case 'B':
	case 'C':
	case 'D':
	case 'E':
	case 'F':
	case 'G':
	case 'H':
	case 'I':
	case 'J':
	case 'K':
	case 'L':
	case 'M':
	case 'N':
	case 'O':
	case 'P':
	case 'Q':
	case 'R':
	case 'S':
	case 'T':
	case 'U':
	case 'V':
	case 'W':
	case 'X':
	case 'Y':
	case 'Z':
		VTControlModeFlag=0;
	default:
		//如果输入的VT参数过长.强行退出VT模式
		if(VTCursorPosion >= MAX_VT_COMMAND_BUFFER_SIZE)
		{
			VTCursorPosion = 0;
			VTControlModeFlag = 0;
			break;
		}
		
		VTCmdBuffer[VTCursorPosion] = sbuftemp;
		VTCursorPosion++;
		break;
	}
	//如果VT模式还没结束则暂时不做后面的处理
	if(VTControlModeFlag!=0||VTCursorPosion<=0)return;
	
	//判断语法是否正确
	if(VTCmdBuffer[0]!='[')
	{
		 VTCursorPosion=0;
		memset(VTCmdBuffer,'\0',MAX_VT_COMMAND_BUFFER_SIZE);
		return;
	}

	if(VTCursorPosion>2)
	{
		for (i = 1; i < VTCursorPosion; i++)
		{
			if (VTCmdBuffer[i]==';')
			{
				argc++;
				continue;
			}
			
			if(VTCmdBuffer[i]>=0x30&&VTCmdBuffer[i]<=0x39)
			{
				argv[argc]=(argv[argc]*10)+(VTCmdBuffer[i]-0x30);
				continue;
			}
			
			if (i==VTCursorPosion-1)
			{
				argc++;
				continue;
			}
		}
	}
	
	switch(VTCmdBuffer[VTCursorPosion-1])
	{
		case 'D'://暂且先当做删除处理
			if(CursorPosion > 0)
			{
				CursorPosion--;
				SendByte(0x08);
				SendByte(' ');
				SendByte(0x08);
			}
			SerialBuffer[CursorPosion] = '\0';
			if(argc>=1)
			{
				//SendUInt(argv[0]);
				//SendStr("\r\n");
			}
			break;
		default:
			break;
	}
	// */
	VTCursorPosion=0;
	memset(VTCmdBuffer,'\0',MAX_VT_COMMAND_BUFFER_SIZE);
}

/* 普通字符键盘输入 */
void CharacterInput(unsigned char sbuftemp)
{
	switch(sbuftemp)
	{
	case 0x1B://ESC
		VTControlModeFlag=1;
		break;
	case 0x08://删除
	case 0x06:
	case 0x07:
	case 0x7E:
	case 0x7F:
		if(CursorPosion > 0)
		{
			CursorPosion--;
			SendByte(0x08);
			SendByte(' ');
			SendByte(0x08);
		}
		SerialBuffer[CursorPosion] = '\0';
		break;
	case '\r':
	case '\n':
	case '\0':
		SendByte('\r');
		SendByte('\n');
		ExecCommandFlag = 1;
		break;
	case '\t':
		break;
	default:
		if(CursorPosion >= MAX_SERIAL_BUFFER_SIZE)
		{
			CursorPosion = 0;
			memset(SerialBuffer,'\0',MAX_SERIAL_BUFFER_SIZE);
			SendStr2("\r\n 警告:您输入的内容过长!\r\n\r\n",F_RED,DEFAULT_B_COLOR);
			SendStr2(PromptBuffer,PROMPT_F_COLOR,DEFAULT_B_COLOR);
			break;
		}
		SerialBuffer[CursorPosion] = sbuftemp;
		SendByte(sbuftemp);
		CursorPosion++;
		break;
	}
}

//串口事件
void SerialInterrupt(void)//interrupt 4 using 3
{
	char sbuftemp;
	if(!RI)return;
	sbuftemp = SBUF;
	RI = 0;
	if(VTControlModeFlag)
	{
		VTInput(sbuftemp);
	}else
	{
		CharacterInput(sbuftemp);
	}
}

/* 解析参数 命令缓冲区 p参数数量 参数数组 */
void ParseArgs(char *argstr,char *argc_p,char **argv, char **resid)
{
	char argc = 0;
	char c;
	//暂存状态,用于转义后恢复
	PARSESTATE stackedstate;
	//上一个符号的状态
	PARSESTATE laststate = PS_WHITESPACE;

	//循环从缓冲区读取字符
	/*
		我们把解析模式分为:普通模式:空白  普通模式:输入中  字符串模式(包含在2个双引号之间)  转义模式(在"\"后面的1个符号)
	*/
	while ((c = *argstr) != 0)
	{
		//新状态(当前符号会按照新状态处理)
		PARSESTATE newstate;
		//如果遇到分号.并且处于普通模式,则结束解析
		if (c == ';' && laststate != PS_STRING && laststate != PS_ESCAPE)break;
		//接下来会根据不同状态,对c的内容进行处理
		if (laststate == PS_ESCAPE)//如果上一个符号是转义转态,将暂存的状态作为新状态,因为转义状态只对单个字符生效
		{
			newstate = stackedstate;
		}
		else if (laststate == PS_STRING)//如果上一个符号是字符串状态
		{
			if (c == '"')//遇到双引号,将新状态设为普通空白,并分割内容
		 	{
				newstate = PS_WHITESPACE;
				*argstr = 0;
			}
		 	else//如果遇到其他符号,则下一个字还是字符串状态
			{
				newstate = PS_STRING;
			}
		}
	 	else if ((c == ' ') || (c == '\t'))//如果遇到空格或者\t 则分割,并且将新状态改为普通空白
		{
			*argstr = 0;
			newstate = PS_WHITESPACE;
		}
	 	else if (c == '"')//如果遇到双引号.则新状态改为字符串
		{
			newstate = PS_STRING;
			*argstr++ = 0;//此处把双引号改为\0 并将地址放进argv
			argv[argc++] = argstr;
		}
	 	else if (c == '\\')//遇到斜杠,进入转义模式(进入前,保存旧状态)
		{
			stackedstate = laststate;
			newstate = PS_ESCAPE;
		}
	 	else //普通模式
		{
			if (laststate == PS_WHITESPACE) argv[argc++] = argstr;
			newstate = PS_TOKEN;
		}
		//将新状态,转移到旧状态,并进入下一个字符
		laststate = newstate;
		argstr++;
	}

	argv[argc] = NULL;
	if (argc_p != NULL)*argc_p = argc;
	if (*argstr == ';') *argstr++ = '\0';
	*resid = argstr;
}

void ExecCommand(char *buf)
{
	char argc,*argv[8],*resid,i;
	COMMAND *Command = 0;

	while(*buf)
 	{
		//清空之前的argv
		memset(argv,0,sizeof(argv));
		//解析参数
		ParseArgs(buf, &argc, argv, &resid);
		//把命令第一节转为小写
		toLower(argv[0]);

		if(argc <=0)
		{
			buf = resid;
			continue;
		}
		for(i = 0; CommandList[i].CommandName!=NULL; i++)
		{
			Command = &CommandList[i];
			if(strncmp(Command->CommandName,argv[0],strlen(argv[0])) == 0)break;
			Command = 0;
		}
		if(Command == 0)
		{
			SendStr("'");
			SendStr2(argv[0],F_RED,DEFAULT_B_COLOR);
			SendStr("' 不是内部或外部命令，也不是可运行的程序.如果需要帮助请输入");
			SendLine2("'help'\r\n",F_YELLOW,DEFAULT_B_COLOR); 
		}
		else
		{
			Argc=argc;
			Argv=argv;
			Command->CommandFunc();
		}
		buf = resid;
	}
}

void RunTerminal(void)
{
	if (RI==1)SerialInterrupt(); 
	if(ExecCommandFlag)
	{
		ExecCommand(SerialBuffer);
		SendStr2(PromptBuffer,PROMPT_F_COLOR,DEFAULT_B_COLOR);
		memset(SerialBuffer,'\0',MAX_SERIAL_BUFFER_SIZE);
		CursorPosion = 0;
		VTCursorPosion = 0;
		ExecCommandFlag = 0;
	}
}