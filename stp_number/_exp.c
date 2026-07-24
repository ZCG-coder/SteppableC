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
    STP_Number truncate_divisor;

    STP_Number_init(&term);
    STP_Number_init(&sum);
    STP_Number_init(&k);
    STP_Number_init(&truncate_divisor);

    if (!STP_Number_set(&truncate_divisor, 1) || !_STP_Number_mul_exp(&truncate_divisor, wp))
        goto fail;

    /* i = 1 */
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

        /* truncate divide */
        term.scale = 0;
        if (!STP_Number_div(&term, &truncate_divisor, 0))
        {
            status = 0;
            break;
        }
        term.scale = -wp;

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
    STP_Number_destroy(&truncate_divisor);

    return status;

fail:
    STP_Number_destroy(&truncate_divisor);
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

    int is_negative = (x_copy.sign < 0);
    x_copy.sign = 1;

    uint64_t k = _STP_Number_bit_count(&x_copy) + 1;
    uint64_t max_x = (1ULL << k);
    uint64_t integer_digits = (max_x * 435ULL) / 1000ULL;
    int64_t wp_internal = wp + integer_digits + 8;

    STP_Number truncate_divisor;
    STP_Number_init(&truncate_divisor);
    STP_Number_set(&truncate_divisor, 1);
    _STP_Number_mul_exp(&truncate_divisor, wp_internal);
    if (k > 0)
    {
        STP_Number divisor;
        if (!STP_Number_init(&divisor))
        {
            STP_Number_destroy(&x_copy);
            return 0;
        }

        if (!STP_Number_set(&divisor, 1) || !STP_Number_lshift(&divisor, k) ||
            !STP_Number_div(&x_copy, &divisor, wp_internal))
        {
            STP_Number_destroy(&divisor);
            STP_Number_destroy(&x_copy);
            return 0;
        }

        STP_Number_destroy(&divisor);
    }

    if (!_STP_Number_exp_taylor(&x_copy, wp_internal))
    {
        STP_Number_destroy(&x_copy);
        return 0;
    }

    for (uint64_t i = 0; i < k; ++i)
    {
        if (!STP_Number_sqr(&x_copy))
        {
            STP_Number_destroy(&truncate_divisor);
            STP_Number_destroy(&x_copy);
            return 0;
        }

        x_copy.scale = 0;
        if (!STP_Number_div(&x_copy, &truncate_divisor, 0))
        {
            STP_Number_destroy(&truncate_divisor);
            STP_Number_destroy(&x_copy);
            return 0;
        }
        x_copy.scale = -wp_internal;
    }
    STP_Number_destroy(&truncate_divisor);

    /* e^-x = 1/(e^x) */
    if (is_negative)
    {
        STP_Number one;
        if (!STP_Number_conv(&one, "1"))
        {
            STP_Number_destroy(&x_copy);
            return 0;
        }

        if (!STP_Number_div(&one, &x_copy, 0))
        {
            STP_Number_destroy(&one);
            STP_Number_destroy(&x_copy);
            return 0;
        }

        STP_Number_destroy(&x_copy);
        x_copy = one;
    }

    STP_Number_round(&x_copy, wp);
    free(x->arr);
    *x = x_copy;

    return 1;
}
