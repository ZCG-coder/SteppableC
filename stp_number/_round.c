#include "_utils.h"
#include "stp_number.h"

#include <stdint.h>
#include <string.h>

int _STP_Number_rough_round(STP_Number* num, int64_t wp)
{
    if (num == NULL || num->arr == NULL || wp < 0)
        return 0;

    /* why round if already integral? */
    if (num->scale >= 0)
        return 1;

    int64_t current_wp = -num->scale;
    if (current_wp <= wp)
        return 1;

    int64_t diff = current_wp - wp;
    uint64_t nblocks = diff / 19;

    /* cannot further remove full block of 19 */
    if (nblocks == 0)
        return 1;

    uint64_t rounding_digit = 0;

    if (nblocks <= num->size)
        rounding_digit = num->arr[nblocks - 1] / 1000000000000000000ULL; /* 10^18 */

    if (nblocks >= num->size)
    {
        num->size = 1;
        num->arr[0] = 0;
        num->scale += (int64_t)nblocks * 19;
    }
    else
    {
        uint64_t new_size = num->size - nblocks;
        memmove(num->arr, num->arr + nblocks, new_size * sizeof(uint64_t));

        num->size = new_size;
        num->scale += (int64_t)nblocks * 19;
    }

    if (rounding_digit >= 5)
        _STP_Number_add(num, 1);

    return 1;
}

void _trunc_div(STP_Number* num, uint64_t P, uint64_t Q)
{
    for (uint64_t i = 0; i < num->size; i++)
    {
        uint64_t lower_part = num->arr[i] / P;
        uint64_t upper_part = 0;
        if (i + 1 < num->size)
        {
            uint64_t high = num->arr[i + 1];
            uint64_t high_mod_P = high - (high / P) * P;
            upper_part = high_mod_P * Q;
        }

        num->arr[i] = lower_part + upper_part;
    }
}

int STP_Number_round(STP_Number* num, int64_t wp)
{
    if (num == NULL || num->arr == NULL || wp < 0)
        return 0;

    /* why round if already integral? */
    if (num->scale >= 0)
        return 1;

    int64_t current_wp = -num->scale;
    if (current_wp <= wp)
        return 1;

    int64_t diff = current_wp - wp;
    int64_t orig_diff = diff;

    uint64_t nblocks = diff / 19;
    uint64_t rem_diff = diff % 19;

    uint64_t rounding_digit = 0;
    uint64_t digit_limb_idx = (diff - 1) / 19;
    uint64_t digit_rem_idx = (diff - 1) % 19;

    if (digit_limb_idx < num->size)
    {
        uint64_t pow_divisor = _POW10[digit_rem_idx];
        rounding_digit = (num->arr[digit_limb_idx] / pow_divisor) % 10;
    }

    if (nblocks > 0)
    {
        if (nblocks >= num->size)
        {
            num->size = 1;
            num->arr[0] = 0;
        }
        else
        {
            uint64_t new_size = num->size - nblocks;
            memmove(num->arr, num->arr + nblocks, new_size * sizeof(uint64_t));
            num->size = new_size;
        }
    }

    if (rem_diff > 0 && !(num->size == 1 && num->arr[0] == 0))
    {
        uint64_t P = _POW10[rem_diff];
        uint64_t Q = _POW10[19 - rem_diff];

        _trunc_div(num, P, Q);
        _STP_Number_trim(num);
    }

    if (rounding_digit >= 5)
        _STP_Number_add(num, 1);

    num->scale += orig_diff;

    if (num->size == 1 && num->arr[0] == 0)
        num->scale = 0;

    return 1;
}
