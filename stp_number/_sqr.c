#include "_shift.h"
#include "_utils.h"
#include "stp_number.h"

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

int _STP_Number_sqr_abs(STP_Number* out, const STP_Number* in)
{
    uint64_t n = in->size;
    if (n == 0)
    {
        out->size = 0;
        return 1;
    }

    if (!_STP_Number_ensure_capacity(out, 2 * n))
        return 0;

    /* multiply off-diagonal cross-terms (i < j) */
    for (uint64_t i = 0; i < n; ++i)
    {
        uint64_t carry = 0;
        for (uint64_t j = i + 1; j < n; ++j)
        {
            uint64_t hi, lo;
            _STP_mul64_wide(in->arr[i], in->arr[j], &hi, &lo);

            uint64_t c1 = _STP_add64_carry(&out->arr[i + j], lo);
            uint64_t c2 = _STP_add64_carry(&out->arr[i + j], carry);
            carry = hi + c1 + c2;
        }

        out->arr[i + n] = carry;
    }

    _raw_shift_left(out->arr, 2 * n, 1);

    for (uint64_t i = 0; i < n; ++i)
    {
        uint64_t hi, lo;
        _STP_mul64_wide(in->arr[i], in->arr[i], &hi, &lo);

        uint64_t c1 = _STP_add64_carry(&out->arr[2 * i], lo);

        uint64_t c2 = _STP_add64_carry(&out->arr[2 * i + 1], hi);
        uint64_t c3 = _STP_add64_carry(&out->arr[2 * i + 1], c1);

        uint64_t ripple = c2 + c3;
        uint64_t idx = 2 * i + 2;
        while (ripple > 0 && idx < 2 * n)
        {
            ripple = _STP_add64_carry(&out->arr[idx], ripple);
            idx++;
        }
    }

    out->size = 2 * n;
    _STP_Number_trim(out);
    return 1;
}

int STP_Number_sqr(STP_Number* num)
{
    if (num == NULL || num->arr == NULL)
        return 0;

    if (STP_Number_is_zero(num))
        return 1;

    STP_Number out;
    if (!STP_Number_init(&out))
        return 0;

    if (!_STP_Number_sqr_abs(&out, num))
    {
        STP_Number_destroy(&out);
        return 0;
    }

    /* sqr is always positive for num in R */
    out.sign = 1;
    out.scale = num->scale * 2;

    free(num->arr);
    *num = out;

    return 1;
}
