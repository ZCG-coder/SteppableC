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
    int64_t orig_diff = current_wp - wp;
    if (current_wp <= wp)
        return 1;

    int64_t diff = current_wp - wp;
    uint64_t last_digit = 0;

    while (diff >= 9)
    {
        uint64_t remainder = _STP_Number_mod(num, 1000000000ULL);
        last_digit = remainder / 100000000ULL;
        diff -= 9;
    }

    for (uint64_t index = 1; diff > 0; index++)
    {
        if (diff & 1ULL)
        {
            uint64_t divisor = _EXPS[index];
            uint64_t remainder = _STP_Number_mod(num, divisor);
            last_digit = remainder / (divisor / 10ULL);
        }
        diff >>= 1;
    }

    if (last_digit >= 5)
        _STP_Number_add(num, 1);

    num->scale += orig_diff;

    return 1;
}
