#include "_mul.h"

#include "_utils.h"
#include "stp_number.h"

#include <stdlib.h>
#include <string.h>

void _STP_mul64_wide(uint64_t lhs, uint64_t rhs, uint64_t* high, uint64_t* low)
{
    uint64_t l_low = lhs & 0xFFFFFFFFULL;
    uint64_t l_high = lhs >> 32;
    uint64_t r_low = rhs & 0xFFFFFFFFULL;
    uint64_t r_high = rhs >> 32;

    uint64_t p_ll = l_low * r_low;
    uint64_t p_lh = l_low * r_high;
    uint64_t p_hl = l_high * r_low;
    uint64_t p_hh = l_high * r_high;

    uint64_t mid = (p_ll >> 32) + (p_lh & 0xFFFFFFFFULL) + (p_hl & 0xFFFFFFFFULL);
    uint64_t carry_mid = (mid >> 32);

    *low = (p_ll & 0xFFFFFFFFULL) | (mid << 32);
    *high = p_hh + (p_lh >> 32) + (p_hl >> 32) + carry_mid;
}

int _STP_Number_mul_abs_schoolbook(STP_Number* out, const STP_Number* lhs, const STP_Number* rhs)
{
    if (out == NULL || lhs == NULL || rhs == NULL)
        return 0;

    if (out->arr == NULL || lhs->arr == NULL || rhs->arr == NULL)
        return 0;

    if (lhs->size == 0 || rhs->size == 0)
    {
        out->size = 0;
        out->scale = 0;
        out->sign = 1;
        return 1;
    }

    if (!_STP_Number_ensure_capacity(out, lhs->size + rhs->size))
        return 0;

    out->size = lhs->size + rhs->size;
    out->scale = 0;
    out->sign = 1;
    memset(out->arr, 0, out->size * sizeof(uint64_t));

    for (uint64_t i = 0; i < lhs->size; ++i)
    {
        uint64_t carry = 0;

        for (uint64_t j = 0; j < rhs->size; ++j)
        {
            uint64_t high = 0;
            uint64_t low = 0;
            uint64_t idx = i + j;
            uint64_t c1 = 0;
            uint64_t c2 = 0;

            _STP_mul64_wide(lhs->arr[i], rhs->arr[j], &high, &low);

            c1 = _STP_add64_carry(&out->arr[idx], low);
            c2 = _STP_add64_carry(&out->arr[idx], carry);

            carry = high + c1 + c2;
        }

        for (uint64_t k = i + rhs->size; carry != 0; ++k)
        {
            if (k >= out->size)
                return 0;

            carry = _STP_add64_carry(&out->arr[k], carry);
        }
    }

    if (!_STP_Number_trim(out))
        return 0;

    if (STP_Number_is_zero(out))
        out->sign = 1;

    return 1;
}

int _STP_Number_mul_abs(STP_Number* out, const STP_Number* lhs, const STP_Number* rhs)
{
    uint64_t n = 0;

    if (out == NULL || lhs == NULL || rhs == NULL)
        return 0;
    if (out->arr == NULL || lhs->arr == NULL || rhs->arr == NULL)
        return 0;

    n = (lhs->size > rhs->size) ? lhs->size : rhs->size;

    if (n < STP_MUL_KARATSUBA_THRESHOLD)
        return _STP_Number_mul_abs_schoolbook(out, lhs, rhs);

    return _STP_Number_mul_abs_karatsuba(out, lhs, rhs);
}

int STP_Number_mul(STP_Number* lhs, STP_Number* rhs)
{
    if (lhs == NULL || rhs == NULL)
        return 0;
    if (lhs->arr == NULL || rhs->arr == NULL)
        return 0;

    if (!_STP_Number_trim(lhs) || !_STP_Number_trim(rhs))
        return 0;

    if (STP_Number_is_zero(lhs))
        return 1;
    if (STP_Number_is_zero(rhs))
    {
        STP_Number_clear(lhs);
        return 1;
    }

    STP_Number out_abs;

    int8_t lhs_sign = 0;
    int8_t rhs_sign = 0;
    int64_t lhs_scale = 0;
    int64_t rhs_scale = 0;
    int8_t final_sign = 1;
    int64_t final_scale = 0;
    lhs_sign = lhs->sign;
    rhs_sign = rhs->sign;
    lhs_scale = lhs->scale;
    rhs_scale = rhs->scale;

    final_sign = (lhs_sign == rhs_sign) ? 1 : -1;
    final_scale = lhs_scale + rhs_scale;

    if (!STP_Number_init(&out_abs))
        return 0;

    lhs->sign = 1;
    rhs->sign = 1;
    lhs->scale = 0;
    rhs->scale = 0;

    if (!_STP_Number_mul_abs(&out_abs, lhs, rhs))
    {
        lhs->sign = lhs_sign;
        lhs->scale = lhs_scale;
        rhs->sign = rhs_sign;
        rhs->scale = rhs_scale;
        STP_Number_destroy(&out_abs);
        return 0;
    }

    out_abs.scale = final_scale;
    out_abs.sign = STP_Number_is_zero(&out_abs) ? 1 : final_sign;

    /* Move out to LHS */
    free(lhs->arr);
    *lhs = out_abs;

    rhs->sign = rhs_sign;
    rhs->scale = rhs_scale;
    return 1;
}
