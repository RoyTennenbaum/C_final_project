#include <stdio.h>
#include <stdlib.h>

typedef struct
{
    char *digits;
    int length;
} quaternaryBaseNum;

quaternaryBaseNum decimalToQuat(int decimal)
{
    quaternaryBaseNum quaternary;
    quaternary.digits = malloc(sizeof(char));
    quaternary.digits[0] = 0;
    quaternary.length = 1;
    int i;
    char temp;

    while (decimal != 0)
    {
        quaternary.digits[quaternary.length - 1] = decimal % 4;
        decimal /= 4;
        if (decimal != 0)
        {
            quaternary.length++;
            quaternary.digits = realloc(quaternary.digits, sizeof(char) * quaternary.length);
        }
    }

    for (i = 0; i < quaternary.length / 2; i++)
    {
        temp = quaternary.digits[i];
        quaternary.digits[i] = quaternary.digits[quaternary.length - 1 - i];
        quaternary.digits[quaternary.length - 1 - i] = temp;
    }
    return quaternary;
}