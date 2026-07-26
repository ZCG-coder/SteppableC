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
    while (remaining_diff >= 16)
    {
        _mul10(num, 10000000000000000ULL); /* 10^16 */
        remaining_diff -= 16;
    }

    /*
     * remaining_diff = 37
     *
     * Pass 1 -> * 10^16, * 10^16
     * remaining_diff = 5
     *
     * Pass 2
     * 5 = 0b101
     *       4 1
     * -> * 10^4, * 10^1
     */
    for (uint64_t index = 1; remaining_diff > 0; index++)
    {
        if (remaining_diff & 1ULL)
        {
            if (!_mul10(num, _EXPS[index]))
                return 0;
        }
        remaining_diff >>= 1;
    }
    return 1;
}

uint64_t _STP_Number_mod(STP_Number* num, uint64_t base)
{
    if (num == NULL || num->arr == NULL || num->size == 0)
        return 0;

    uint64_t remainder = 0;

    for (int64_t i = (int64_t)num->size - 1; i >= 0; i--)
    {
        uint64_t current_block = num->arr[i];

        uint64_t high_half = current_block >> 32;
        uint64_t low_half = current_block & 0xFFFFFFFF;

        uint64_t temp_high = (remainder << 32) | high_half;
        uint64_t quot_high = temp_high / base;
        remainder = temp_high % base;

        uint64_t temp_low = (remainder << 32) | low_half;
        uint64_t quot_low = temp_low / base;
        remainder = temp_low % base;

        num->arr[i] = (quot_high << 32) | quot_low;
    }

    _STP_Number_trim(num);
    return remainder;
}

uint8_t _STP_Number_mod10(STP_Number* num) { return (uint8_t)_STP_Number_mod(num, 10); }

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
