#include "Terminal.h"
#include "Commands.h"
/*****************************************************************************/
char idata SerialBuffer[MAX_SERIAL_BUFFER_SIZE];
char idata VTCmdBuffer[MAX_VT_COMMAND_BUFFER_SIZE];//
char code PromptBuffer[]=">";
char idata CursorPosion;/* 命令行输入缓冲区指针 */
char idata LastCursorPosion=0;/* 上次刷新显示时的命令行输入缓冲区指针 */
char idata VTCursorPosion;/* VT命令缓冲区指针 */
char idata ExecCommandFlag;/* 执行命令标志 */
char idata WaitingDisplayFlag=0;/* 等待刷新显示 */
char idata VTControlModeFlag;/* VT控制模式 */
long int GapCounter=0;
long int Counter=0;
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
	case '~':
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
	//判断是否有参数,如有则对其解析
	if(VTCursorPosion>2)
	{
		for (i = 1; i < VTCursorPosion; i++)
		{
			if (VTCmdBuffer[i]==';'||VTCmdBuffer[i]==',')
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
	//SendLine(VTCmdBuffer);
	//执行VT命令
	switch(VTCmdBuffer[VTCursorPosion-1])
	{
		case 'D'://光标左移
			if(CursorPosion > 0)
			{
				CursorPosion--;
				LastCursorPosion--;
				SendByte(0x08);
			}
			break;
		case 'C'://光标右移
			if(SerialBuffer[CursorPosion] != '\0')
			{
				LastCursorPosion++;
				CursorPosion++;
				SendStr("\033[C");
			}
			break;
		case '~'://按键信息
			if(argc==1)
			{
				if(argv[0]==1)//Home
				{
					//if(SerialBuffer[CursorPosion] == 0)break;
					SendStr("\033[");
					SendUInt(CursorPosion);
					SendStr("D");
					CursorPosion=0;
					break;
				}
				if(argv[0]==3)//Delete
				{
					if(SerialBuffer[CursorPosion] == 0)break;
					//记录下游标位置,然后内容前移,恢复游标位置
					SendStr(SAVECURSOR);
					//在内存里输出光标后的内容,此时会覆盖光标里的字,相当于原地删除了
					SendStr(&SerialBuffer[CursorPosion+1]);
					SendByte(0x20);//在末尾输出个空格,覆盖末尾的字留下的残影.
					SendStr(RESTORECURSOR);//恢复光标位置
					//在内存里把光标后面的内容全部往前移一格(包括末尾的\0)
					for (i = CursorPosion; i < strlen(SerialBuffer); i++)SerialBuffer[i]=SerialBuffer[i+1];
					break;
				}
				if(argv[0]==4)//End
				{
					if(SerialBuffer[CursorPosion] == 0)break;
					SendStr("\033[");
					SendUInt(strlen(SerialBuffer)-CursorPosion);
					SendStr("C");
					CursorPosion=strlen(SerialBuffer);
					break;
				}
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
	unsigned char temp=0;
	int i;
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
		if(CursorPosion <= 0)break;
		LastCursorPosion--;
		CursorPosion--;
		//首先左移一下游标,然后记录下位置
		SendByte(0x08);
		SendStr(SAVECURSOR);
		//在内存里输出光标后的内容,此时会覆盖光标里的字,相当于原地删除了
		SendStr(&SerialBuffer[CursorPosion+1]);
		SendByte(0x20);//在末尾输出个空格,覆盖末尾的字留下的残影.
		SendStr(RESTORECURSOR);//恢复光标位置
		//在内存里把光标后面的内容全部往前移一格(包括末尾的\0)
		for (i = CursorPosion; i < strlen(SerialBuffer); i++)SerialBuffer[i]=SerialBuffer[i+1];
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
		//如果用户输入过长的内容,清空缓冲区并提示
		temp=strlen(SerialBuffer);
		if(temp >= MAX_SERIAL_BUFFER_SIZE)
		{
			LastCursorPosion = CursorPosion = 0;
			memset(SerialBuffer,'\0',MAX_SERIAL_BUFFER_SIZE);
			SendStr2("\r\n 警告:您输入的内容过长!\r\n\r\n",F_RED,DEFAULT_B_COLOR);
			SendStr2(PromptBuffer,PROMPT_F_COLOR,DEFAULT_B_COLOR);
			break;
		}
		
		//把当前位置以及之后的内容全部后移,然后在当前位置的字符填入用户的按键
		for (i = temp-1; i >= CursorPosion; i--)SerialBuffer[i+1]=SerialBuffer[i];
		SerialBuffer[CursorPosion] = sbuftemp;
		CursorPosion++;
		WaitingDisplayFlag=1;//设置一个标志,等串口空闲了再刷新界面显示
		// */
		break;
	}
}

void SerialInterrupt() interrupt 4 using 1
{
	char sbuftemp;
	if(RI==0)return;
	sbuftemp = SBUF;
	RI=0;
	SerialInput(sbuftemp);
}

//串口事件
void SerialInput(char sbuftemp)
{
	P11=!P11;
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
			//SendUInt(Counter);
			//SendLine("\r\n");
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
	char sbuftemp;
	if (RI==1)
	{
		GapCounter=0;
		sbuftemp = SBUF;
		RI=0;
		SerialInput(sbuftemp);
	}
	//在串口操作里面无法及时响应,如果串口有连续空闲,则刷新显示
	if(RI==0)GapCounter++;
	if(GapCounter>=128&&WaitingDisplayFlag==1)
	{
		GapCounter=0;
		WaitingDisplayFlag=0; 
		SendStr(SAVECURSOR);
		SendStr(&SerialBuffer[LastCursorPosion]);
		SendStr(RESTORECURSOR);
		if(LastCursorPosion>CursorPosion)
		{
			SendStr("\033[");
			SendUInt(LastCursorPosion-CursorPosion);
			SendStr("D");
		}
		else if(LastCursorPosion<CursorPosion)
		{
			SendStr("\033[");
			SendUInt(CursorPosion-LastCursorPosion);
			SendStr("C");
		}
		// */
		LastCursorPosion=CursorPosion;
	}
	// */
	if(ExecCommandFlag)
	{
		ExecCommand(SerialBuffer);
		SendStr2(PromptBuffer,PROMPT_F_COLOR,DEFAULT_B_COLOR);
		memset(SerialBuffer,'\0',MAX_SERIAL_BUFFER_SIZE);
		CursorPosion = 0;
		VTCursorPosion = 0;
		ExecCommandFlag = 0;
		LastCursorPosion=0;
	}
}