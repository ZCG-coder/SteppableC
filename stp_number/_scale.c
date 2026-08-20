#include "_utils.h"
#include "stp_number.h"

#include <stdint.h>
#include <string.h>

int _mul10(STP_Number* num, uint64_t fac)
{
    if (num == NULL || num->arr == NULL)
        return 0;
    if (fac == 1)
        return 1;
    if (fac == 0)
    {
        num->arr[0] = 0;
        num->size = 1;
        return 1;
    }

    uint64_t D = _BASE_10_19 / fac;
    uint64_t carry = 0;

    for (uint64_t i = 0; i < num->size; ++i)
    {
        uint64_t block = num->arr[i];

        uint64_t high = block / D;
        uint64_t low = block % D;

        /* (low * fac) + carry < 10^19 <= UINT64_MAX */
        num->arr[i] = (low * fac) + carry;
        carry = high;
    }

    if (carry > 0)
    {
        if (!_STP_Number_ensure_capacity(num, num->size + 1))
            return 0;

        num->arr[num->size++] = carry;
    }

    return 1;
}

int _STP_Number_mul_exp(STP_Number* num, uint64_t diff_scale)
{
    if (num == NULL || num->arr == NULL)
        return 0;
    if (diff_scale == 0)
        return 1;

    uint64_t remaining_diff = diff_scale;
    while (remaining_diff >= 19)
    {
        _mul10(num, 10000000000000000000ULL); /* 10^19 */
        remaining_diff -= 19;
    }

    if (remaining_diff > 0)
    {
        if (!_mul10(num, _POW10[remaining_diff]))
            return 0;
    }
    return 1;
}

int _STP_Number_align_scales(STP_Number* lhs, STP_Number* rhs)
{
    if (lhs == NULL || rhs == NULL)
        return 0;
    if (lhs->arr == NULL || rhs->arr == NULL)
        return 0;
    if (lhs == rhs)
        return 0;

    /* Align scales */
    if (lhs->scale > rhs->scale)
    {
        uint64_t diff = lhs->scale - rhs->scale;
        _STP_Number_mul_exp(lhs, diff);
        lhs->scale = rhs->scale;
    }
    else if (lhs->scale < rhs->scale)
    {
        uint64_t diff = rhs->scale - lhs->scale;
        _STP_Number_mul_exp(rhs, diff);
        rhs->scale = lhs->scale;
    }

    return 1;
}
