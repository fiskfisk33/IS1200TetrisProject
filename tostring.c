/*
 * C Program which Converts an Integer to String & vice-versa
 */
#include <string.h>
 
void tostring(char [], int);
 
 
void tostring(char str[], int num)
{

    int i, rem, len = 0, n;
 
    n = num;
    while (n != 0)
    {
        len++;
        n /= 10;
    }
    for (i = 0; i < len; i++)
    {
        rem = num % 10;
        num = num / 10;
        str[len - (i + 1)] = rem + '0';
    }
    str[len] = '\0';
    if(str[0] == '\0'){
        str[0] = '0';
        str[1] = '\0';
    }
}

void to_string(char *c, int num){
        
}