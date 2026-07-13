#include "_utils.h"
#include "helpers.h"
#include "stp_number.h"

#include <stdint.h>

uint64_t _raw_mul_sub(uint64_t* a, const uint64_t* b, uint64_t b_len, uint64_t q)
{
    /*
     * Computes a = a - (b * q)
     */
    if (q == 0)
        return 0;

    uint64_t multiplication_carry = 0;
    uint64_t subtraction_borrow = 0;

    for (uint64_t i = 0; i < b_len; i++)
    {
        uint64_t prod_high = 0;
        uint64_t prod_low = 0;

        _STP_mul64_wide(b[i], q, &prod_high, &prod_low);

        uint64_t internal_carry = _STP_add64_carry(&prod_low, multiplication_carry);
        multiplication_carry = prod_high + internal_carry;

        uint64_t total_subtract_amount = prod_low;
        uint64_t borrow_overflow = _STP_add64_carry(&total_subtract_amount, subtraction_borrow);

        uint64_t next_borrow = (a[i] < total_subtract_amount) ? 1 : 0;
        a[i] -= total_subtract_amount;

        subtraction_borrow = next_borrow + borrow_overflow;
    }

    uint64_t total_guard_subtract = multiplication_carry;
    uint64_t guard_borrow_overflow = _STP_add64_carry(&total_guard_subtract, subtraction_borrow);

    uint64_t final_underflow = (a[b_len] < total_guard_subtract) ? 1 : 0;
    a[b_len] -= total_guard_subtract;

    return final_underflow + guard_borrow_overflow;
}

void _estimate_q_hat(uint64_t u_high, uint64_t u_low, uint64_t v1, uint64_t* q_hat, uint64_t* r_hat)
{
    /*
     *   ^    U0 * 2^64 + U1
     *   q = ----------------
     *              V1
     */
    if (u_high >= v1)
    {
        *q_hat = 0xFFFFFFFFFFFFFFFFULL;
        *r_hat = u_low + v1;
        return;
    }

    uint64_t v1_high = v1 >> 32;
    uint64_t v1_low = v1 & 0xFFFFFFFFULL;

    uint64_t q1 = u_high / v1_high;
    uint64_t r1 = u_high % v1_high;

    while (q1 >= 0x100000000ULL || q1 * v1_low > (r1 << 32) + (u_low >> 32))
    {
        q1--;
        r1 += v1_high;
        if (r1 >= v1_high)
            break;
    }

    uint64_t rem = ((u_high << 32) | (u_low >> 32)) - q1 * v1;

    uint64_t q0 = rem / v1_high;
    uint64_t r0 = rem % v1_high;

    while (q0 >= 0x100000000ULL || q0 * v1_low > (r0 << 32) + (u_low & 0xFFFFFFFFULL))
    {
        q0--;
        r0 += v1_high;
        if (r0 >= v1_high)
            break;
    }

    *q_hat = (q1 << 32) | q0;
    *r_hat = (rem << 32) | (u_low & 0xFFFFFFFFULL) - (*q_hat) * v1;
}

int STP_Number_mod(STP_Number* lhs, const STP_Number* rhs, STP_Number* quotient)
{
    if (lhs == NULL || rhs == NULL)
        return 0;
    if (lhs->arr == NULL || rhs->arr == NULL)
        return 0;
    if (STP_Number_is_zero(rhs))
    {
        fprintf(stderr, "%s: div by zero!\n", STP_CURRENT_FUNCTION);
        return 0;
    }
    if (STP_Number_is_zero(lhs))
    {
        if (quotient != NULL)
            STP_Number_clear(quotient);
        return 1;
    }

    int final_q_sign = (lhs->sign == rhs->sign) ? 1 : -1;
    int final_r_sign = lhs->sign;

    if (_STP_Number_cmp_abs(lhs, rhs) < 0)
    {
        if (quotient)
        {
            STP_Number_clear(quotient);
            quotient->sign = final_q_sign;
        }
        lhs->sign = final_r_sign;
        return 1;
    }

    STP_Number V;
    STP_Number_init(&V);
    if (!STP_Number_copy(rhs, &V))
        return 0;
    V.sign = 1;

    uint64_t top_v = V.arr[V.size - 1];
    uint32_t d = 0;
    while ((top_v & (1ULL << 63)) == 0)
    {
        d++;
        top_v <<= 1;
    }

    lhs->sign = 1;
    if (d > 0)
    {
        if (!STP_Number_lshift(lhs, d) || !STP_Number_lshift(&V, d))
        {
            STP_Number_destroy(&V);
            return 0;
        }
    }

    if (!_STP_Number_ensure_capacity(lhs, lhs->size + 1))
    {
        STP_Number_destroy(&V);
        return 0;
    }
    lhs->arr[lhs->size] = 0;
    lhs->size++;

    uint64_t n = V.size;
    uint64_t m = lhs->size - 1 - n;

    STP_Number Q;
    if (quotient)
    {
        STP_Number_init(&Q);
        if (!_STP_Number_ensure_capacity(&Q, m + 1))
        {
            STP_Number_destroy(&V);
            return 0;
        }
        memset(Q.arr, 0, (m + 1) * sizeof(uint64_t));
        Q.size = m + 1;
    }

    uint64_t v1 = V.arr[n - 1];
    uint64_t v2 = V.arr[n - 2];

    for (int64_t j = m; j >= 0; j--)
    {
        uint64_t u0 = lhs->arr[j + n];
        uint64_t u1 = lhs->arr[j + n - 1];
        uint64_t u2 = lhs->arr[j + n - 2];

        uint64_t q_hat = 0;
        uint64_t r_hat = 0;

        _estimate_q_hat(u0, u1, v1, &q_hat, &r_hat);

        while (1)
        {
            uint64_t prod_high = 0;
            uint64_t prod_low = 0;
            _STP_mul64_wide(q_hat, v2, &prod_high, &prod_low);

            if (prod_high > r_hat || (prod_high == r_hat && prod_low > u2))
            {
                q_hat--;
                uint64_t old_r_hat = r_hat;
                r_hat += v1;
                if (r_hat < old_r_hat)
                    break;
                continue;
            }
            break;
        }

        uint64_t borrow = _raw_mul_sub(&lhs->arr[j], V.arr, n, q_hat);

        /* Underflow compensation */
        if (borrow > 0)
        {
            q_hat--;
            uint64_t carry = 0;
            for (uint64_t i = 0; i < n; i++)
            {
                uint64_t sum = lhs->arr[j + i] + V.arr[i];
                uint64_t c1 = (sum < lhs->arr[j + i]) ? 1 : 0;

                uint64_t final_sum = sum + carry;
                uint64_t c2 = (final_sum < sum) ? 1 : 0;

                lhs->arr[j + i] = final_sum;
                carry = c1 + c2;
            }
            lhs->arr[j + n] += carry;
        }

        /* Save the confirmed block if compiling a quotient */
        if (quotient != NULL)
            Q.arr[j] = q_hat;
    }

    lhs->size = n;
    if (d > 0)
        STP_Number_rshift(lhs, d);

    lhs->sign = final_r_sign;
    _STP_Number_trim(lhs);

    /* Save quotient */
    if (quotient)
    {
        _STP_Number_trim(&Q);
        Q.sign = final_q_sign;
        Q.scale = lhs->scale;

        free(quotient->arr);
        *quotient = Q;
    }

    STP_Number_destroy(&V);
    return 1;
}

int STP_Number_div(STP_Number* lhs, const STP_Number* rhs, uint64_t decimal_places)
{
    if (lhs == NULL || rhs == NULL)
        return 0;
    if (lhs->arr == NULL || rhs->arr == NULL)
        return 0;
    if (STP_Number_is_zero(rhs))
    {
        fprintf(stderr, "%s: div by zero!", STP_CURRENT_FUNCTION);
        return 0;
    }

    int64_t target_scale = (int64_t)lhs->scale + decimal_places - (int64_t)rhs->scale;
    if (target_scale < 0)
        target_scale = 0;

    if (decimal_places > 0)
    {
        if (!_STP_Number_mul_exp(lhs, decimal_places))
            return 0;
    }

    STP_Number quotient;
    STP_Number remainder_scratch;

    STP_Number_init(&quotient);
    STP_Number_init(&remainder_scratch);

    if (!STP_Number_copy(lhs, &remainder_scratch))
        goto quotient_fail;

    if (!STP_Number_mod(&remainder_scratch, rhs, &quotient))
        goto fail;

    if (!STP_Number_lshift(&remainder_scratch, 1))
        goto fail;

    if (_STP_Number_cmp_abs(&remainder_scratch, rhs) >= 0)
    {
        if (!_STP_Number_add(&quotient, 1))
            goto fail;
    }

    STP_Number_destroy(&remainder_scratch);

    quotient.scale = -target_scale;
    _STP_Number_trim(&quotient);

    /* Move quotient to LHS */
    free(lhs->arr);
    *lhs = quotient;

    return 1;

fail:
    STP_Number_destroy(&remainder_scratch);
quotient_fail:
    STP_Number_destroy(&quotient);

    /* Poison LHS to disallow further operations */
    free(lhs->arr);
    lhs->arr = NULL;
    lhs->size = 0;
    lhs->scale = 0;
    return 0;
}
