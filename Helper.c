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
int parseHex(char * hex,char * buff,int maxlenght)
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

unsigned int crc16(unsigned char *ptr,unsigned int count)
{
    unsigned int  crc;
    unsigned char i;
    crc = 0;
    while (--count >= 0)
    {
        crc = crc ^ (unsigned int) *ptr++ << 8;
        i = 8;
        do
        {
            if (crc & 0x8000)
            {
                crc = crc << 1 ^ 0x1021;
            }
            else
            {
                crc = crc << 1;
            }
        } while(--i);
    }
    return (crc);
}

unsigned int checksum16(unsigned char *dataptr,unsigned int len)
{
    unsigned long int acc;
    unsigned int src;
    unsigned char *octetptr;

    acc = 0;
    octetptr = (unsigned char*)dataptr;
    while (len > 1) {
        src = (*octetptr) << 8;
        octetptr++;
        src |= (*octetptr);
        octetptr++;
        acc += src;
        len -= 2;
    }
    if (len > 0) {
        src = (*octetptr) << 8;
        acc += src;
    }

    acc = (acc >> 16) + (acc & 0x0000ffffUL);
    if ((acc & 0xffff0000UL) != 0) acc = (acc >> 16) + (acc & 0x0000ffffUL);

    src = (unsigned int)acc;
    return ~src;
}

unsigned char checksum8(unsigned char *pBuffer, unsigned int size)
{
    unsigned char cksum=0;
    int i=0;
    if ((NULL == pBuffer) || (0 == size))return 0;

    for (i = 0; i < size; i++)
    {
        cksum+=pBuffer[i];
    }
    return cksum;
}