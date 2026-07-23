#include "stp_number.h"

#include <string.h>
#include <struct.h>

int STP_Number_abs(STP_Number* num)
{
    if (num == NULL || num->arr == NULL)
        return 0;

    if (num->sign < 0)
        num->sign = -num->sign;
    return 1;
}
