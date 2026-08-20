#include "_mul.h"

#include "_utils.h"
#include "config.h"
#include "stp_number.h"

#include <stdlib.h>
#include <string.h>

void _mul_add(uint64_t A, uint64_t B, uint64_t C, uint64_t K, uint64_t* out_digit, uint64_t* out_carry)
{
    /* change 0 in next line to INT128_ENABLED if your CPU supports native int128 division */
#if 0 /* hardware int128 might be slow for division */
    __uint128_t prod = (__uint128_t)A * B + C + K;

    *out_digit = (uint64_t)(prod % _BASE_10_19);
    *out_carry = (uint64_t)(prod / _BASE_10_19);
#else
    /* No int128 support, split A, B, C, K -> 3 base-10^9 blocks */
    uint64_t a_hi = A / _BASE_10_9;
    uint64_t a0 = A - a_hi * _BASE_10_9;
    uint64_t a2 = a_hi / _BASE_10_9;
    uint64_t a1 = a_hi - a2 * _BASE_10_9;

    uint64_t b_hi = B / _BASE_10_9;
    uint64_t b0 = B - b_hi * _BASE_10_9;
    uint64_t b2 = b_hi / _BASE_10_9;
    uint64_t b1 = b_hi - b2 * _BASE_10_9;

    uint64_t p0 = a0 * b0;
    uint64_t p1 = a0 * b1 + a1 * b0;
    uint64_t p2 = a0 * b2 + a1 * b1 + a2 * b0;
    uint64_t p3 = a1 * b2 + a2 * b1;
    uint64_t p4 = a2 * b2;

    uint64_t p0_hi = p0 / _BASE_10_9;
    uint64_t d0 = p0 - p0_hi * _BASE_10_9;
    p1 += p0_hi;

    uint64_t p1_hi = p1 / _BASE_10_9;
    uint64_t d1 = p1 - p1_hi * _BASE_10_9;
    p2 += p1_hi;

    uint64_t p2_hi = p2 / _BASE_10_9;
    uint64_t d2 = p2 - p2_hi * _BASE_10_9;
    p3 += p2_hi;

    uint64_t p3_hi = p3 / _BASE_10_9;
    uint64_t d3 = p3 - p3_hi * _BASE_10_9;
    p4 += p3_hi;

    uint64_t d2_carry = d2 / 10;
    uint64_t d2_rem = d2 - d2_carry * 10;

    uint64_t ab_digit = d0 + (d1 * _BASE_10_9) + (d2_rem * 1000000000000000000ULL /* 10^18 */);
    uint64_t ab_carry = d2_carry + (d3 * 100000000ULL /* 10^8 */) + (p4 * 100000000000000000ULL /* 10^17 */);

    uint64_t diff;
    /* add C */
    diff = _BASE_10_19 - ab_digit;
    if (C >= diff)
    {
        ab_digit = C - diff;
        ab_carry++;
    }
    else
    {
        ab_digit += C;
    }

    /* add K */
    diff = _BASE_10_19 - ab_digit;
    if (K >= diff)
    {
        ab_digit = K - diff;
        ab_carry++;
    }
    else
    {
        ab_digit += K;
    }

    *out_digit = ab_digit;
    *out_carry = ab_carry;
#endif
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

    for (uint64_t i = 0; i < lhs->size; ++i)
    {
        uint64_t carry = 0;

        for (uint64_t j = 0; j < rhs->size; ++j)
        {
            uint64_t digit = 0;
            _mul_add(lhs->arr[i], rhs->arr[j], out->arr[i + j], carry, &digit, &carry);
            out->arr[i + j] = digit;
        }

        out->arr[i + rhs->size] = carry;
    }

    if (!_STP_Number_trim(out))
        return 0;

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

    int8_t lhs_sign = lhs->sign;
    int8_t rhs_sign = rhs->sign;
    int64_t lhs_scale = lhs->scale;
    int64_t rhs_scale = rhs->scale;

    int8_t final_sign = (lhs_sign == rhs_sign) ? 1 : -1;
    int64_t final_scale = lhs_scale + rhs_scale;

    if (!STP_Number_init_capacity(&out_abs, lhs->size + rhs->size))
        return 0;

    lhs->sign = 1;
    rhs->sign = 1;
    lhs->scale = 0;
    rhs->scale = 0;

    int status = _STP_Number_mul_abs(&out_abs, lhs, rhs);

    lhs->sign = lhs_sign;
    lhs->scale = lhs_scale;
    rhs->sign = rhs_sign;
    rhs->scale = rhs_scale;

    if (!status)
    {
        STP_Number_destroy(&out_abs);
        return 0;
    }
    out_abs.scale = final_scale;
    out_abs.sign = STP_Number_is_zero(&out_abs) ? 1 : final_sign;

    /* Move out to LHS */
    free(lhs->arr);
    *lhs = out_abs;

    return 1;
}
