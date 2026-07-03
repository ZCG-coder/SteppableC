#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*

How this works

Number -> bits

eg. 5,423,365,189.268
->

[ 5 4 2 3 3 6 5 1 8 9 128 2 6 8 ]

 */

typedef struct
{
    uint64_t used_bits;
    int8_t* arr;
} STP_Number;

int STP_Number_init(STP_Number* num)
{
    if (num == NULL)
        return -1;

    num->arr = (int8_t*)malloc(1);
    memset(num->arr, 1, 0);

    num->used_bits = 1;

    return 0;
}

int STP_Number_destroy(STP_Number* num)
{
    if (num == NULL)
        return -1;

    free(num->arr);
    num->arr = NULL;
    num->used_bits = 0;

    return 0;
}

void STP_Number_toChar(const STP_Number* num, STP_String* str)
{
    str->length = num->used_bits;

    for (uint64_t i = 0; i < num->used_bits; ++i)
    {
    }
}

int main()
{
    STP_Number n;
    (void)STP_Number_init(&n);
    STP_Number_destroy(&n);
    return 0;
}
