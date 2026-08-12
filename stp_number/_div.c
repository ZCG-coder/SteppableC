#include "_div.h"

#include "_utils.h"
#include "helpers.h"
#include "stp_number.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef STP_DIV_NEWTON
int STP_Number_div(STP_Number* lhs, STP_Number* rhs, uint64_t decimal_places)
{
    int64_t target_exp;
    int8_t final_sign;
    STP_Number tmp_rhs;
    STP_Number tmp_q;

    if (lhs == NULL || rhs == NULL)
        return 0;
    if (lhs->arr == NULL || rhs->arr == NULL)
        return 0;

    if (STP_Number_cmp(lhs, rhs) == 0)
    {
        /* lhs == rhs, set lhs to 1 */
        return STP_Number_set(lhs, 0x1ULL);
    }

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

    final_sign = (lhs->sign == rhs->sign) ? 1 : -1;

    /* calculate the net scaling factor required to achieve the exact target scale */
    target_exp = lhs->scale - rhs->scale + (int64_t)decimal_places;
    if (!STP_Number_init_capacity(&tmp_rhs, rhs->size - ((target_exp < 0) ? target_exp : 0)))
        return 0;
    if (!STP_Number_copy(rhs, &tmp_rhs))
        goto tmp_rhs_fail;

    if (target_exp > 0)
    {
        if (!_STP_Number_mul_exp(lhs, (uint64_t)target_exp))
            goto tmp_rhs_fail;
    }
    else if (target_exp < 0)
    {
        if (!_STP_Number_mul_exp(&tmp_rhs, (uint64_t)(-target_exp)))
            goto tmp_rhs_fail;
    }

    if (!STP_Number_init_capacity(&tmp_q, lhs->size - tmp_rhs.size))
        goto tmp_rhs_fail;
    /* execute the absolute value base division */
    STP_Number r;
    if (!STP_Number_init_capacity(&r, tmp_rhs.size))
        goto tmp_q_fail;

    if (!_STP_Number_div_abs(lhs, &tmp_rhs, &tmp_q, &r))
        goto r_fail;

    /* r *= 2 */
    if (!STP_Number_add(&r, &r))
        goto r_fail;
    /* if remainder > 1/2 of rhs, round answer off */
    if (_STP_Number_cmp_abs(&r, &tmp_rhs) == 1)
        if (!_STP_Number_add(&tmp_q, 1))
            goto r_fail;

    free(lhs->arr);
    *lhs = tmp_q;
    lhs->scale = -(int64_t)decimal_places;
    lhs->sign = final_sign;

    STP_Number_destroy(&tmp_rhs);
    STP_Number_destroy(&r);

    return 1;

r_fail:
    STP_Number_destroy(&r);
tmp_q_fail:
    STP_Number_destroy(&tmp_q);
tmp_rhs_fail:
    STP_Number_destroy(&tmp_rhs);
    return 0;
}
#endif
