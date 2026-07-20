#include "_utils.h"
#include "helpers.h"
#include "stp_number.h"

#include <stdint.h>
#include <string.h>

/* Helper to extract a 32-bit digit from a little-endian uint64_t array */
uint32_t _STP_get_digit32(const STP_Number* num, uint64_t idx)
{
    uint64_t block = idx / 2;
    if (block >= num->size)
        return 0;
    if (idx % 2 == 0)
        return (uint32_t)(num->arr[block] & 0xFFFFFFFFULL);
    return (uint32_t)(num->arr[block] >> 32);
}

/* Helper to write a base-2^32 stream back into an initialized STP_Number */
int _STP_pack_from_32(STP_Number* dest, const uint32_t* src32, uint64_t len32)
{
    if (len32 == 0)
    {
        dest->size = 1;
        if (!_STP_Number_ensure_capacity(dest, 1))
            return 0;
        dest->arr[0] = 0;
        return 1;
    }

    uint64_t len64 = (len32 + 1) / 2;
    if (!_STP_Number_ensure_capacity(dest, len64))
        return 0;
    dest->size = len64;

    for (uint64_t i = 0; i < len64; i++)
    {
        uint64_t low = src32[i * 2];
        uint64_t high = ((i * 2 + 1) < len32) ? (uint64_t)src32[i * 2 + 1] : 0ULL;
        dest->arr[i] = low | (high << 32);
    }
    return _STP_Number_trim(dest);
}

int _STP_Number_div_abs(const STP_Number* lhs, const STP_Number* rhs, STP_Number* quotient, STP_Number* remainder)
{
    if (lhs == NULL || rhs == NULL)
        return 0;
    if (lhs->arr == NULL || rhs->arr == NULL)
        return 0;

    /* Find true sizes in 32-bit digit lengths, ignoring leading zeros */
    uint64_t u_len = lhs->size * 2;
    while (u_len > 0 && _STP_get_digit32(lhs, u_len - 1) == 0)
        u_len--;

    uint64_t v_len = rhs->size * 2;
    while (v_len > 0 && _STP_get_digit32(rhs, v_len - 1) == 0)
        v_len--;

    if (v_len == 0)
    {
        fprintf(stderr, "%s: Division by zero\n", STP_CURRENT_FUNCTION);
        return 0;
    }

    /* Base Case 1: Dividend < Divisor */
    if (u_len < v_len || (u_len == v_len && _STP_Number_cmp_abs(lhs, rhs) < 0))
    {
        if (quotient && !STP_Number_clear(quotient))
            return 0;
        if (remainder && !STP_Number_copy(lhs, remainder))
            return 0;
        return 1;
    }

    /* Base Case 2: Divisor is a single 32-bit digit (Short Division) */
    if (v_len == 1)
    {
        uint32_t divisor32 = _STP_get_digit32(rhs, 0);
        uint32_t* q32 = (uint32_t*)calloc(u_len, sizeof(uint32_t));
        if (q32 == NULL)
        {
            fprintf(stderr, "%s: Memory allocation failed\n", STP_CURRENT_FUNCTION);
            return 0;
        }

        uint64_t rem32 = 0;
        for (int64_t i = (int64_t)u_len - 1; i >= 0; i--)
        {
            uint64_t current = (rem32 << 32) | _STP_get_digit32(lhs, i);
            q32[i] = (uint32_t)(current / divisor32);
            rem32 = current % divisor32;
        }

        if (quotient && !_STP_pack_from_32(quotient, q32, u_len))
        {
            free(q32);
            return 0;
        }
        if (remainder)
        {
            if (!STP_Number_clear(remainder))
            {
                free(q32);
                return 0;
            }
            remainder->arr[0] = rem32;
        }

        free(q32);
        return 1;
    }

    /* General Case: Knuth Algorithm D for multi-digit divisors */
    uint32_t v_high = _STP_get_digit32(rhs, v_len - 1);
    int s = 0;
    while ((v_high & (uint32_t)0x80000000) == 0)
    {
        v_high <<= 1;
        s++;
    }

    /* Allocate normalized workspace arrays */
    uint32_t* v_norm = (uint32_t*)calloc(v_len, sizeof(uint32_t));
    uint32_t* u_norm = (uint32_t*)calloc(u_len + 1, sizeof(uint32_t));
    uint64_t m = u_len - v_len;
    uint32_t* q32 = (uint32_t*)calloc(m + 1, sizeof(uint32_t));

    if (!v_norm || !u_norm || !q32)
    {
        fprintf(stderr, "%s: Memory allocation failed\n", STP_CURRENT_FUNCTION);
        free(v_norm);
        free(u_norm);
        free(q32);
        return 0;
    }

    /* Normalize inputs via bit shift stream maps */
    uint64_t carry = 0;
    for (uint64_t i = 0; i < v_len; i++)
    {
        uint64_t val = ((uint64_t)_STP_get_digit32(rhs, i) << s) | carry;
        v_norm[i] = (uint32_t)(val & 0xFFFFFFFFULL);
        carry = val >> 32;
    }

    carry = 0;
    for (uint64_t i = 0; i < u_len; i++)
    {
        uint64_t val = ((uint64_t)_STP_get_digit32(lhs, i) << s) | carry;
        u_norm[i] = (uint32_t)(val & 0xFFFFFFFFULL);
        carry = val >> 32;
    }
    u_norm[u_len] = (uint32_t)carry;

    /* Main loop over quotient digits */
    for (int64_t j = (int64_t)m; j >= 0; j--)
    {
        /* 1. Calculate estimated quotient digit (q_hat) */
        uint64_t num_hat = ((uint64_t)u_norm[j + v_len] << 32) | u_norm[j + v_len - 1];
        uint64_t q_hat = num_hat / v_norm[v_len - 1];
        uint64_t r_hat = num_hat % v_norm[v_len - 1];

    _loop:
        if (q_hat >= 0x100000000ULL || q_hat * v_norm[v_len - 2] > ((r_hat << 32) | u_norm[j + v_len - 2]))
        {
            q_hat--;
            r_hat += v_norm[v_len - 1];
            if (r_hat < 0x100000000ULL)
                goto _loop;
        }

        /* 2. Multiply and subtract block segments */
        uint64_t borrow = 0;
        for (uint64_t i = 0; i < v_len; i++)
        {
            uint64_t prod = q_hat * v_norm[i] + borrow;
            uint64_t diff = u_norm[j + i];
            if (diff < (prod & 0xFFFFFFFFULL))
            {
                u_norm[j + i] = (uint32_t)(diff + 0x100000000ULL - (prod & 0xFFFFFFFFULL));
                borrow = (prod >> 32) + 1;
            }
            else
            {
                u_norm[j + i] = (uint32_t)(diff - (prod & 0xFFFFFFFFULL));
                borrow = (prod >> 32);
            }
        }

        /* 3. Test for a negative balance borrow condition */
        if (u_norm[j + v_len] < borrow)
        {
            u_norm[j + v_len] = (uint32_t)(u_norm[j + v_len] + 0x100000000ULL - borrow);
            q_hat--;
            uint64_t carry_back = 0;
            for (uint64_t i = 0; i < v_len; i++)
            {
                uint64_t sum = (uint64_t)u_norm[j + i] + v_norm[i] + carry_back;
                u_norm[j + i] = (uint32_t)(sum & 0xFFFFFFFFULL);
                carry_back = sum >> 32;
            }
            u_norm[j + v_len] = (uint32_t)((u_norm[j + v_len] + carry_back) & 0xFFFFFFFFULL);
        }
        else
        {
            u_norm[j + v_len] -= (uint32_t)borrow;
        }

        q32[j] = (uint32_t)q_hat;
    }

    /* Handle packing results and true remainder denormalization */
    if (quotient && !_STP_pack_from_32(quotient, q32, m + 1))
    {
        free(v_norm);
        free(u_norm);
        free(q32);
        return 0;
    }

    if (remainder != NULL)
    {
        uint32_t* r32 = (uint32_t*)calloc(v_len, sizeof(uint32_t));
        if (!r32)
        {
            free(v_norm);
            free(u_norm);
            free(q32);
            return 0;
        }

        if (s == 0)
        {
            for (uint64_t i = 0; i < v_len; i++)
                r32[i] = u_norm[i];
        }
        else
        {
            uint32_t carry_r = 0;
            for (int64_t i = (int64_t)v_len - 1; i >= 0; i--)
            {
                uint32_t next_carry = u_norm[i] << (32 - s);
                r32[i] = (u_norm[i] >> s) | carry_r;
                carry_r = next_carry;
            }
        }

        int pack_ok = _STP_pack_from_32(remainder, r32, v_len);
        free(r32);
        if (!pack_ok)
        {
            free(v_norm);
            free(u_norm);
            free(q32);
            return 0;
        }
    }

    free(v_norm);
    free(u_norm);
    free(q32);
    return 1;
}

int STP_Number_div(STP_Number* lhs, const STP_Number* rhs, uint64_t decimal_places)
{
    int64_t target_exp;
    int8_t final_sign;
    STP_Number tmp_rhs;
    STP_Number tmp_q;

    if (lhs == NULL || rhs == NULL)
    {
        fprintf(stderr, "%s: Null pointer passed\n", STP_CURRENT_FUNCTION);
        return 0;
    }

    if (STP_Number_is_zero(rhs))
    {
        fprintf(stderr, "%s: Division by zero\n", STP_CURRENT_FUNCTION);
        return 0;
    }

    if (STP_Number_is_zero(lhs))
    {
        lhs->scale = -(int64_t)decimal_places;
        lhs->sign = 1;
        return 1;
    }

    final_sign = (lhs->sign == rhs->sign) ? 1 : -1;

    if (!STP_Number_init(&tmp_rhs))
        return 0;

    if (!STP_Number_copy(rhs, &tmp_rhs))
        goto tmp_rhs_fail;

    if (!STP_Number_init(&tmp_q))
        goto tmp_rhs_fail;

    /* Calculate the net scaling factor required to achieve the exact target scale */
    target_exp = lhs->scale - tmp_rhs.scale + (int64_t)decimal_places;

    if (target_exp > 0)
    {
        if (!_STP_Number_mul_exp(lhs, (uint64_t)target_exp))
            goto fail;
    }
    else if (target_exp < 0)
    {
        if (!_STP_Number_mul_exp(&tmp_rhs, (uint64_t)(-target_exp)))
            goto fail;
    }

    /* Execute the absolute value base division */
    STP_Number r;
    if (!STP_Number_init(&r))
        goto fail;

    if (!_STP_Number_div_abs(lhs, &tmp_rhs, &tmp_q, &r))
        goto fail;

    /* r *= 2*/
    if (!STP_Number_lshift(&r, 1))
        goto r_fail;
    if (STP_Number_cmp(&r, &tmp_rhs) == 1)
        if (!_STP_Number_add(&tmp_q, 1))
            goto r_fail;

    if (!STP_Number_copy(&tmp_q, lhs))
        goto r_fail;

    lhs->scale = -(int64_t)decimal_places;
    lhs->sign = final_sign;

    STP_Number_destroy(&tmp_rhs);
    STP_Number_destroy(&tmp_q);
    STP_Number_destroy(&r);

    return 1;

r_fail:
    STP_Number_destroy(&r);
fail:
    STP_Number_destroy(&tmp_q);
tmp_rhs_fail:
    STP_Number_destroy(&tmp_rhs);
    return 0;
}
