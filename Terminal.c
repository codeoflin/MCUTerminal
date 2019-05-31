#include "Terminal.h"
#include "Commands.h"
/*****************************************************************************/
char idata SerialBuffer[MAX_SERIAL_BUFFER_SIZE];
char idata VTCmdBuffer[MAX_VT_COMMAND_BUFFER_SIZE];
char idata PromptBuffer[MAX_PROMPT_BUFFER_SIZE];
char idata CursorPosion;/* 命令行输入缓冲区指针 */
char idata VTCursorPosion;/* VT命令缓冲区指针 */
char idata ExecCommandFlag;/* 执行命令标志 */
char idata VTControlMode;/* VT控制模式 */

/*****************************************************************************/
void InitTerminal(void)
{
	int i;
	for(i = 0; CommandList[i].CommandName!=NULL; i++)toLower(CommandList[i].CommandName);
	CursorPosion = 0;
	ExecCommandFlag = 0;
	VTControlMode = 0;
	memset(&SerialBuffer[0],'\0',MAX_SERIAL_BUFFER_SIZE);
	memcpy(&PromptBuffer[0],">",MAX_PROMPT_BUFFER_SIZE);
	SendStr(DEFAULT_F_Color);
	SendStr(DEFAULT_B_Color);
	SendStr(CLEARSCREEN);
	SendStr(CURSORHOME);
	SendStr("*****************************\r\n");
	SendStr2("     Linz Terminal System    \r\n",F_RED,B_BLACK);
	SendStr2("        林子51终端系统       \r\n",F_BLUE,B_BLACK);
	SendStr("*****************************\r\n");
	SendStr("\r\n");
	SendStr2(&PromptBuffer[0],PROMPT_F_Color,B_BLACK);
}

//VT输入
void VTInput(unsigned char sbuftemp)
{
	int argv[]={0,0,0,0,0,0,0,0};//其实vt参数最多3个
	int argc=0;
	int i;
	//SendUInt(VTCursorPosion);
	//SendStr(" Input:");
	//SendHexByte(sbuftemp);
	//SendStr("\r\n");
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
		VTControlMode=0;
	default:
		if(VTCursorPosion >= MAX_VT_COMMAND_BUFFER_SIZE)
		{
			VTCursorPosion = 0;
			VTControlMode = 0;
			break;
		}
		
		VTCmdBuffer[VTCursorPosion] = sbuftemp;
		VTCursorPosion++;
		break;
	}
	//如果VT模式还没结束则暂时不做后面的处理
	if(VTControlMode!=0||VTCursorPosion<=0)return;
	
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
			//SendStr("delete!");
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

//普通字符键盘输入
void CharacterInput(unsigned char sbuftemp)
{
	switch(sbuftemp)
	{
		
	case 0x1B://ESC
		//SendStr("Enter VTMode\r\n");
		VTControlMode=1;
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
	case '\r'://
	case '\n'://
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
			memset(&SerialBuffer[0],'\0',MAX_SERIAL_BUFFER_SIZE);
			SendStr2("\r\n 警告:您输入的内容过长!\r\n\r\n",F_RED,B_BLACK);
			SendStr2(&PromptBuffer[0],PROMPT_F_Color,B_BLACK);
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
	if(VTControlMode)
	{
		VTInput(sbuftemp);
	}else
	{
		CharacterInput(sbuftemp);
	}
}

void ParseArgs(char *argstr,char *argc_p,char **argv, char **resid)
{
	char argc = 0;
	char c;
	PARSESTATE stackedState,lastState = PS_WHITESPACE;

	while ((c = *argstr) != 0)
	{
		PARSESTATE newState;

		if (c == ';' && lastState != PS_STRING && lastState != PS_ESCAPE)break;

		if (lastState == PS_ESCAPE)
		{
			newState = stackedState;
		}
		else if (lastState == PS_STRING)
		{
			if (c == '"')
		 	{
				newState = PS_WHITESPACE;
				*argstr = 0;
			}
		 	else 
			{
				newState = PS_STRING;
			}
		}
	 	else if ((c == ' ') || (c == '\t'))
		{
			*argstr = 0;
			newState = PS_WHITESPACE;
		}
	 	else if (c == '"') 
		{
			newState = PS_STRING;
			*argstr++ = 0;
			argv[argc++] = argstr;
		}
	 	else if (c == '\\') 
		{
			stackedState = lastState;
			newState = PS_ESCAPE;
		}
	 	else 
		{
			if (lastState == PS_WHITESPACE) 
			{
				argv[argc++] = argstr;
			}
			newState = PS_TOKEN;
		}

		lastState = newState;
		argstr++;
	}

	argv[argc] = NULL;
	if (argc_p != NULL)
		*argc_p = argc;

	if (*argstr == ';') 
	{
		*argstr++ = '\0';
	}
	*resid = argstr;
}

void ExecCommand(char *buf)
{
	char argc,*argv[8],*resid,i;
	COMMAND *Command = 0;

	while(*buf)
 	{
		memset(argv,0,sizeof(argv));
		ParseArgs(buf, &argc, argv, &resid);
		toLower(argv[0]);
		if(argc > 0)
		{
			for(i = 0; CommandList[i].HelpString!=NULL; i++)
			{
				Command = &CommandList[i];
				if(strncmp(Command->CommandName,argv[0],strlen(argv[0])) == 0)break;
				Command = 0;
			}
			if(Command == 0)
			{
				SendStr2(argv[0],F_RED,B_BLACK);
				SendStr(" 不是内部或外部命令，也不是可运行的程序.如果需要帮助请输入");
				SendStr2("'help'\r\n\r\n",F_YELLOW,B_BLACK); 
		   	}
			else
			{
				Argc=argc;
				Argv=argv;
				Command->CommandFunc();
			}
		}
		// */
		buf = resid;
	}
}

void RunTerminal(void)
{
	if (RI==1)SerialInterrupt();
	if(ExecCommandFlag)
	{
		ExecCommand(SerialBuffer);
		SendStr2(&PromptBuffer[0],PROMPT_F_Color,B_BLACK);
		memset(SerialBuffer,'\0',MAX_SERIAL_BUFFER_SIZE);
		CursorPosion = 0;
		VTCursorPosion = 0;
		ExecCommandFlag = 0;
	}
	
}