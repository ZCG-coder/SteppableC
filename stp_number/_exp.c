#include "_utils.h"
#include "stp_number.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int _STP_Number_exp_taylor(STP_Number* num, int64_t wp)
{
    if (num == NULL || num->arr == NULL)
        return 0;

    /*
    exp(x) = 1 + x + x^2/2! + x^3/3! + ...
    */

    STP_Number term;
    STP_Number sum;
    STP_Number k;

    STP_Number_init(&term);
    STP_Number_init(&sum);
    STP_Number_init(&k);

    if (!STP_Number_set(&sum, 1) || !STP_Number_copy(num, &term) || !STP_Number_add(&sum, &term))
        goto fail;

    int status = 1;

    for (uint64_t i = 2;; ++i)
    {
        if (!STP_Number_mul(&term, num))
        {
            status = 0;
            break;
        }

        STP_Number_round(&term, wp);

        if (!STP_Number_set(&k, i) || !STP_Number_div(&term, &k, wp))
        {
            status = 0;
            break;
        }

        /* sufficient */
        if (STP_Number_is_zero(&term))
            break;

        if (!STP_Number_add(&sum, &term))
        {
            status = 0;
            break;
        }
    }

    if (status)
    {
        free(num->arr);
        *num = sum;
    }
    else
    {
        STP_Number_destroy(&sum);
    }

    STP_Number_destroy(&term);
    STP_Number_destroy(&k);

    return status;

fail:
    STP_Number_destroy(&term);
    STP_Number_destroy(&sum);
    STP_Number_destroy(&k);
    return 0;
}

int STP_Number_exp(STP_Number* x, int64_t wp)
{
    if (x == NULL || x->arr == NULL)
        return 0;

    if (STP_Number_is_zero(x))
        return STP_Number_set(x, 1);

    STP_Number x_copy;
    STP_Number_init(&x_copy);
    if (!STP_Number_copy(x, &x_copy))
    {
        STP_Number_destroy(&x_copy);
        return 0;
    }

    uint64_t integer_digits = _STP_Number_count_digits(&x_copy);
    uint64_t msb = x_copy.arr[x_copy.size - 1];
    while (msb >= 10)
        msb /= 10;
    uint64_t leading_digit = msb;

    int64_t safe_bound = leading_digit + 1;
    for (uint64_t i = 1; i < integer_digits; ++i)
        safe_bound *= 10;

    int64_t extra_dp = (safe_bound * 435ULL) / 1000ULL;
    int64_t wp_internal = wp + integer_digits + extra_dp + 8;
    x_copy.scale -= integer_digits;

    if (!_STP_Number_exp_taylor(&x_copy, wp_internal))
    {
        STP_Number_destroy(&x_copy);
        return 0;
    }
    x_copy.sign = 1;

    STP_Number x2, x4;
    STP_Number_init(&x2);
    STP_Number_init(&x4);
    for (int64_t i = 0; i < integer_digits; ++i)
    {
        if (!STP_Number_copy(&x_copy, &x2) || !STP_Number_sqr(&x2))
            goto exp_reconstruct_fail;
        STP_Number_round(&x2, wp_internal);

        if (!STP_Number_copy(&x2, &x4) || !STP_Number_sqr(&x4))
            goto exp_reconstruct_fail;
        STP_Number_round(&x4, wp_internal);

        if (!STP_Number_mul(&x_copy, &x4))
            goto exp_reconstruct_fail;
        STP_Number_round(&x_copy, wp_internal);

        if (!STP_Number_sqr(&x_copy))
            goto exp_reconstruct_fail;
        STP_Number_round(&x_copy, wp_internal);
        continue;

    exp_reconstruct_fail:
        STP_Number_destroy(&x2);
        STP_Number_destroy(&x4);
        STP_Number_destroy(&x_copy);
        return 0;
    }
    STP_Number_destroy(&x2);
    STP_Number_destroy(&x4);

    STP_Number_round(&x_copy, wp);
    free(x->arr);
    *x = x_copy;

    return 1;
}
