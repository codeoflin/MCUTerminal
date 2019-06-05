#include "Helper.h"
/* 字符串转小写 */
void toLower(unsigned char* str)
{
    int i;
    if(str==0)return;
    for (i = 0; str[i]!='\0'; i++)
    {
        if(str[i]>='A'&&str[i]<='Z')str[i]=str[i]+0x20;
    }
}

/* line输入的字符串 */
long int toLong(unsigned char *line)
{    
    long int value=0;
    int i;
    int n=strlen(line);
    int temp=0;
    if (n == 0) return -1;
    toLower(line);
    if(line[0]=='0'&&line[1]=='x')
    {
        for (i = 2; line[i]!='\0'; i++)
        {
            temp=index("0123456789abcdef",line[i]);
            if(temp==-1)return -1;
            value=(value*0x10)+temp;
        }
        return value;
    }
    for (; n--; line++) {
        //继续获取下一个字符
        if (*line < '0' || *line > '9')return -1;
		/*
			1、当前字符值 *line的ascii码，减去字符0的ascii码，得出个位数字
			2、原计算的value值 乘以10，向上提升一位
			3、二者相加得到新的十进制数值
		*/
        value = (value * 10) + (*line - '0');
    }
    return (value < 0?-1:value);
}

int index(char *str,char c)
{
    int i=0;
    for (i = 0; str[i]!='\0'; i++)
    {
        if(str[i]==c)return i;
    }
    return -1;
}

/*
解析hex字符串的数据到buff
hex字符串
缓冲区
最大长度
返回值: 解析长度
*/
int ParseHEX(char * hex,char * buff,int maxlenght)
{
    //是否有内容
    char flag_token=0;
    int i=0;
    //缓冲区指针
    int buffposition;
    char temp;
    toLower(hex);
    for(i = 0; hex[i] !=0; i++)
    {
        if(hex[i]==' '|hex[i]=='-')
        {
            if(flag_token!=0)
            {
                buffposition++;
                flag_token=0;
                if(buffposition>maxlenght)return buffposition;
            }
            continue;
        }
        flag_token=1;
        temp=index("0123456789abcdef",hex[i]);
        if(temp<0)return -1;
        buff[buffposition]=(buff[buffposition]*16)+temp;
    }
    if(flag_token!=0)buffposition++;
    return buffposition;
}