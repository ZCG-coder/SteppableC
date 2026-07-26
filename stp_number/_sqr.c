#include "_mul.h"
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

    /* Initialize the output array to zero */
    for (uint64_t i = 0; i < 2 * n; ++i)
        out->arr[i] = 0;

    /* Standard O(n^2) multiplication using the base-10^19 helper */
    for (uint64_t i = 0; i < n; ++i)
    {
        uint64_t carry = 0;
        for (uint64_t j = 0; j < n; ++j)
        {
            uint64_t digit = 0;
            /* out->arr[i+j] acts as 'C' (current value to add to), carry acts as 'K' */
            _mul_add(in->arr[i], in->arr[j], out->arr[i + j], carry, &digit, &carry);
            out->arr[i + j] = digit;
        }
        out->arr[i + n] = carry;
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
