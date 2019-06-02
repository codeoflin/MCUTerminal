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