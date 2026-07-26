#include "_div.h"
#include "_utils.h"
#include "helpers.h"
#include "stp_number.h"

#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

void _initial_guess(uint64_t M, int64_t K, int64_t scale_B, STP_Number* guess)
{
    const long double LOG10_2 = 0.301029995663981195213738894724493026768L;
    long double log_M = log10l((long double)M);
    long double log_value = -log_M - ((long double)K * LOG10_2) - (long double)scale_B;

    long double new_scale_flt = floorl(log_value);
    long double fractional_part = log_value - new_scale_flt;

    long double mantissa = powl(10.0L, fractional_part);
    long double scaled_mantissa = mantissa * 1e15L;

    STP_Number_set(guess, (uint64_t)scaled_mantissa);
    guess->scale = (int64_t)new_scale_flt - 15;
}

#ifdef STP_DIV_NEWTON
int STP_Number_div(STP_Number* lhs, const STP_Number* rhs, uint64_t decimal_places)
{
    if (lhs == NULL || rhs == NULL)
        return 0;
    if (lhs->arr == NULL || rhs->arr == NULL)
        return 0;

    if (lhs == rhs)
        return STP_Number_set(lhs, 0x1ULL);

    if (STP_Number_is_zero(rhs))
    {
        fprintf(stderr, "%s: division by zero\n", STP_CURRENT_FUNCTION);
        return 0;
    }

    if (STP_Number_is_zero(lhs))
    {
        lhs->scale = -(int64_t)decimal_places;
        lhs->sign = 1;
        return 1;
    }

    uint8_t final_sign = (lhs->sign == rhs->sign) ? 1 : -1;

    STP_Number x, two, temp_B, temp_sub;
    STP_Number_init(&x);
    STP_Number_init(&two);
    STP_Number_init(&temp_B);
    STP_Number_init(&temp_sub);

    STP_Number_set(&two, 2);
    uint64_t top_block = rhs->arr[rhs->size - 1];

    int lz = _clz64(top_block);
    uint64_t M = top_block;
    if (lz > 0 && lz < 64)
    {
        M = top_block << lz;
        /* pull in bits from the next block if available */
        if (rhs->size > 1)
            M |= (rhs->arr[rhs->size - 2] >> (64 - lz));
    }

    int64_t true_msb = ((int64_t)rhs->size - 1) * 64 + (63 - lz);
    int64_t K = true_msb - 63;

    _initial_guess(M, K, rhs->scale, &x);
    x.sign = 1;

    uint64_t guard_digits = 5;
    uint64_t current_wp = 15;
    uint64_t lhs_sig_digits = lhs->size * 19;
    uint64_t target_wp = decimal_places + lhs_sig_digits;

    while (current_wp < target_wp + guard_digits)
    {
        current_wp *= 2;
        if (current_wp > target_wp + guard_digits)
            current_wp = target_wp + guard_digits;

        STP_Number_copy(rhs, &temp_B); /* temp_B = B */
        STP_Number_mul(&temp_B, &x); /* temp_B = B * x */
        temp_B.sign = 1;

        STP_Number_copy(&two, &temp_sub); /* temp_sub = 2 */
        STP_Number_sub(&temp_sub, &temp_B); /* temp_sub = 2 - (B * x) */

        STP_Number_mul(&x, &temp_sub); /* x = x * (2 - B * x) */

        STP_Number_round(&x, target_wp + current_wp);
    }

    STP_Number_mul(lhs, &x); /* lhs = A * (1/B) */
    STP_Number_round(lhs, decimal_places);

    lhs->sign = final_sign;

    STP_Number_destroy(&x);
    STP_Number_destroy(&two);
    STP_Number_destroy(&temp_B);
    STP_Number_destroy(&temp_sub);
    return 1;
}
#endif
