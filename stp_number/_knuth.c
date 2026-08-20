#include "_utils.h"
#include "config.h"
#include "helpers.h"
#include "stp_number.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void _mul_64x64(uint64_t a, uint64_t b, uint64_t* hi, uint64_t* lo)
{
#if INT128_ENABLED
    __uint128_t res = (__uint128_t)a * b;
    *lo = (uint64_t)res;
    *hi = (uint64_t)(res >> 64);
#else
    uint64_t a_lo = (uint32_t)a, a_hi = a >> 32;
    uint64_t b_lo = (uint32_t)b, b_hi = b >> 32;

    uint64_t p0 = a_lo * b_lo;
    uint64_t p1 = a_lo * b_hi;
    uint64_t p2 = a_hi * b_lo;
    uint64_t p3 = a_hi * b_hi;

    uint64_t cross = (p0 >> 32) + (uint32_t)p1 + (uint32_t)p2;
    *lo = (p0 & 0xFFFFFFFFULL) | (cross << 32);
    *hi = p3 + (p1 >> 32) + (p2 >> 32) + (cross >> 32);
#endif
}

uint64_t _div_128x64(uint64_t n_hi, uint64_t n_lo, uint64_t d, uint64_t* rem)
{
#if INT128_ENABLED
    __uint128_t n = ((__uint128_t)n_hi << 64) | n_lo;
    *rem = (uint64_t)(n % d);

    return (uint64_t)(n / d);
#else
    if (d == 0)
    {
        if (rem)
            *rem = 0;
        return 0;
    }

    uint64_t q = 0;
    uint64_t r = n_hi;

    for (int i = 63; i >= 0; i--)
    {
        uint64_t bit = (n_lo >> i) & 1;
        uint64_t r_top_bit = r >> 63; /* track MSB before shift to prevent overflow loss */
        r = (r << 1) | bit;
        q <<= 1;

        if (r_top_bit || r >= d)
        {
            r -= d;
            q |= 1;
        }
    }

    if (rem)
        *rem = r;
    return q;
#endif
}

int _STP_Number_div_abs(const STP_Number* lhs, const STP_Number* rhs, STP_Number* quotient, STP_Number* remainder)
{
    uint64_t u_len = lhs->size;
    while (u_len > 0 && lhs->arr[u_len - 1] == 0)
        u_len--;

    uint64_t v_len = rhs->size;
    while (v_len > 0 && rhs->arr[v_len - 1] == 0)
        v_len--;

    if (v_len == 0)
    {
        fprintf(stderr, "%s: division by zero\n", STP_CURRENT_FUNCTION);
        return 0;
    }

    if (u_len < v_len || (u_len == v_len && _STP_Number_cmp_abs(lhs, rhs) < 0))
    {
        if (quotient && !STP_Number_clear(quotient))
            return 0;
        if (remainder && !STP_Number_copy(lhs, remainder))
            return 0;
        return 1;
    }

    if (v_len == 1)
    {
        uint64_t divisor = rhs->arr[0];

        if (quotient)
        {
            if (!_STP_Number_ensure_capacity(quotient, u_len))
                return 0;
            quotient->size = u_len;
        }

        uint64_t rem = 0;
        for (int64_t i = (int64_t)u_len - 1; i >= 0; i--)
        {
            uint64_t cur_hi, cur_lo;
            _mul_64x64(rem, _BASE_10_19, &cur_hi, &cur_lo);
            cur_lo += lhs->arr[i];
            if (cur_lo < lhs->arr[i])
                cur_hi++;

            uint64_t next_rem;
            uint64_t q_digit = _div_128x64(cur_hi, cur_lo, divisor, &next_rem);

            if (quotient)
                quotient->arr[i] = q_digit;
            rem = next_rem;
        }

        if (quotient)
            _STP_Number_trim(quotient);
        if (remainder)
        {
            if (!STP_Number_clear(remainder))
                return 0;
            remainder->arr[0] = rem;
            if (rem > 0)
                remainder->size = 1;
        }
        return 1;
    }

    uint64_t d = _BASE_10_19 / (rhs->arr[v_len - 1] + 1);

    uint64_t* v_norm = (uint64_t*)calloc(v_len, sizeof(uint64_t));
    uint64_t* u_norm = (uint64_t*)calloc(u_len + 1, sizeof(uint64_t));
    uint64_t m = u_len - v_len;
    uint64_t* q_arr = (uint64_t*)calloc(m + 1, sizeof(uint64_t));

    if (v_norm == NULL || u_norm == NULL || q_arr == NULL)
    {
        STP_ERRMSG(STP_CURRENT_FUNCTION, errno);
        goto fail;
    }

    /* normalize */
    uint64_t carry = 0;
    for (uint64_t i = 0; i < v_len; i++)
    {
        uint64_t prod_hi, prod_lo;
        _mul_64x64(rhs->arr[i], d, &prod_hi, &prod_lo);
        prod_lo += carry;
        if (prod_lo < carry)
            prod_hi++;
        carry = _div_128x64(prod_hi, prod_lo, _BASE_10_19, &v_norm[i]);
    }

    carry = 0;
    for (uint64_t i = 0; i < u_len; i++)
    {
        uint64_t prod_hi, prod_lo;
        _mul_64x64(lhs->arr[i], d, &prod_hi, &prod_lo);
        prod_lo += carry;
        if (prod_lo < carry)
            prod_hi++;
        carry = _div_128x64(prod_hi, prod_lo, _BASE_10_19, &u_norm[i]);
    }
    u_norm[u_len] = carry;

    for (int64_t j = (int64_t)m; j >= 0; j--)
    {
        uint64_t num_hi, num_lo;
        _mul_64x64(u_norm[j + v_len], _BASE_10_19, &num_hi, &num_lo);

        num_lo += u_norm[j + v_len - 1];
        if (num_lo < u_norm[j + v_len - 1])
            num_hi++; /* Handle carry */

        uint64_t r_hat;
        uint64_t q_hat = _div_128x64(num_hi, num_lo, v_norm[v_len - 1], &r_hat);

    _loop:
        if (q_hat == _BASE_10_19)
        {
            q_hat--;
            uint64_t old_r_hat = r_hat;
            r_hat += v_norm[v_len - 1];

            if (r_hat >= old_r_hat && r_hat < _BASE_10_19)
                goto _loop;
        }
        else
        {
            uint64_t lhs_hi, lhs_lo;
            _mul_64x64(q_hat, v_norm[v_len - 2], &lhs_hi, &lhs_lo);

            uint64_t rhs_hi, rhs_lo;
            _mul_64x64(r_hat, _BASE_10_19, &rhs_hi, &rhs_lo);
            rhs_lo += u_norm[j + v_len - 2];
            if (rhs_lo < u_norm[j + v_len - 2])
                rhs_hi++; /* Handle carry */

            if (lhs_hi > rhs_hi || (lhs_hi == rhs_hi && lhs_lo > rhs_lo))
            {
                q_hat--;
                uint64_t old_r_hat = r_hat;
                r_hat += v_norm[v_len - 1];

                if (r_hat >= old_r_hat && r_hat < _BASE_10_19)
                    goto _loop;
            }
        }

        uint64_t borrow = 0;
        for (uint64_t i = 0; i < v_len; i++)
        {
            uint64_t prod_hi, prod_lo;
            _mul_64x64(q_hat, v_norm[i], &prod_hi, &prod_lo);
            prod_lo += borrow;
            if (prod_lo < borrow)
                prod_hi++;

            uint64_t p_digit;
            borrow = _div_128x64(prod_hi, prod_lo, _BASE_10_19, &p_digit);

            if (u_norm[j + i] < p_digit)
            {
                u_norm[j + i] = u_norm[j + i] + _BASE_10_19 - p_digit;
                borrow++;
            }
            else
            {
                u_norm[j + i] -= p_digit;
            }
        }

        if (u_norm[j + v_len] < borrow)
        {
            u_norm[j + v_len] = u_norm[j + v_len] + _BASE_10_19 - borrow;
            q_hat--;
            uint64_t carry_back = 0;
            for (uint64_t i = 0; i < v_len; i++)
            {
                uint64_t sum = u_norm[j + i] + v_norm[i] + carry_back;
                if (sum >= _BASE_10_19)
                {
                    u_norm[j + i] = sum - _BASE_10_19;
                    carry_back = 1;
                }
                else
                {
                    u_norm[j + i] = sum;
                    carry_back = 0;
                }
            }
            u_norm[j + v_len] += carry_back;
        }
        else
        {
            u_norm[j + v_len] -= borrow;
        }

        q_arr[j] = q_hat;
    }

    if (quotient)
    {
        if (!_STP_Number_ensure_capacity(quotient, m + 1))
            goto fail;
        quotient->size = m + 1;
        for (uint64_t i = 0; i <= m; i++)
            quotient->arr[i] = q_arr[i];
        _STP_Number_trim(quotient);
    }

    if (remainder)
    {
        if (!_STP_Number_ensure_capacity(remainder, v_len))
            goto fail;
        remainder->size = v_len;
        uint64_t rem_carry = 0;
        for (int64_t i = (int64_t)v_len - 1; i >= 0; i--)
        {
            uint64_t cur_hi, cur_lo;
            _mul_64x64(rem_carry, _BASE_10_19, &cur_hi, &cur_lo);
            cur_lo += u_norm[i];
            if (cur_lo < u_norm[i])
                cur_hi++;

            uint64_t next_carry;
            remainder->arr[i] = _div_128x64(cur_hi, cur_lo, d, &next_carry);
            rem_carry = next_carry;
        }
        _STP_Number_trim(remainder);
    }

    free(v_norm);
    free(u_norm);
    free(q_arr);
    return 1;

fail:
    free(v_norm);
    free(u_norm);
    if (q_arr)
        free(q_arr);
    return 0;
}
