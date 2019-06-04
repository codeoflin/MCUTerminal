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
long int  toLong(unsigned char *line)
{    
    long int  value,n=strlen(line);
    if (n == 0) return -1;
 
    for (value = 0; n--; line++) {
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