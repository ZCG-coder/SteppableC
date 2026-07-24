#include "_utils.h"
#include "stp_number.h"

#include <stdint.h>
#include <string.h>

int STP_Number_round(STP_Number* num, int64_t wp)
{
    if (num == NULL || num->arr == NULL)
        return 0;
    if (wp < 0)
        return 0;

    /* why round if already integral? */
    if (num->scale >= 0)
        return 1;

    int64_t current_wp = -num->scale;
    if (current_wp < wp)
        return 1;

    int64_t diff = current_wp - wp;
    uint8_t last_digit = 0;
    for (int64_t i = 0; i < diff; ++i)
        last_digit = _STP_Number_mod10(num);
    if (last_digit >= 5)
        _STP_Number_add(num, 1);

    num->scale += diff;

    return 1;
}
