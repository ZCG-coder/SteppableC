#include "_utils.h"
#include "stp_number.h"

#include <math.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

int _atan(STP_Number* num_inv, uint64_t iters, int64_t wp)
{
    if (num_inv == NULL)
        return 0;

    /* inverse number */
    STP_Number x;
    STP_Number_conv(&x, "1");
    STP_Number_div(&x, num_inv, wp + 10);

    STP_Number term;
    STP_Number sum;
    STP_Number_init(&term);
    STP_Number_init(&sum);
    STP_Number_copy(&x, &term);
    STP_Number_copy(&x, &sum);

    STP_Number tmp;
    STP_Number_init(&tmp);

    /* x^2 */
    STP_Number_sqr(&x);
    _STP_Number_rough_round(&x, wp + 10);

    STP_Number denom;
    STP_Number_init(&denom);

    for (uint64_t n = 1; n < iters; ++n)
    {
        /* term = -term * x^2 */
        term.sign = -term.sign;
        STP_Number_mul(&term, &x);

        /* sum += term / (2n + 1) */
        STP_Number_set(&denom, 2 * n + 1);
        STP_Number_copy(&term, &tmp);
        STP_Number_div(&tmp, &denom, wp + 10);
        STP_Number_add(&sum, &tmp);
    }

    free(num_inv->arr);
    *num_inv = sum;
    STP_Number_destroy(&tmp);
    STP_Number_destroy(&denom);
    STP_Number_destroy(&term);
    STP_Number_destroy(&x);
    return 1;
}

int STP_Number_pi(STP_Number* out, int64_t wp)
{
    if (out == NULL)
        return 0;

    STP_Number a, b, t, p;

    /* a = 1 */
    STP_Number_init(&a);
    STP_Number_set(&a, 1);

    /* b = 1/sqrt(2) */
    STP_Number_conv(&b, "0.5");
    STP_Number_sqrt(&b, wp + 10);

    /* t = 1/4 */
    STP_Number_conv(&t, "0.25");

    /* p = 1 */
    STP_Number_init(&p);
    STP_Number_set(&p, 1);

    STP_Number two;
    STP_Number_init(&two);
    STP_Number_set(&two, 2);

    STP_Number a_next;
    STP_Number_init(&a_next);

    double iters_d = (double)wp;
    iters_d = log2(iters_d);
    uint64_t iterations = (uint64_t)iters_d + 1;

    for (uint64_t i = 0; i < iterations; ++i)
    {
        /* a_next = (a + b) / 2 */
        STP_Number_copy(&a, &a_next);
        STP_Number_add(&a_next, &b);
        STP_Number_div(&a_next, &two, wp + 10);

        /* b = sqrt(a * b) */
        STP_Number_mul(&b, &a);
        STP_Number_sqrt(&b, wp + 10);

        /* t -= p * (a - a_next)^2 */
        STP_Number_sub(&a, &a_next);
        STP_Number_sqr(&a);
        _STP_Number_rough_round(&a, wp + 10);

        STP_Number_mul(&a, &p);
        _STP_Number_rough_round(&a, wp + 10);

        STP_Number_sub(&t, &a);

        /* a = a_next */
        STP_Number_copy(&a_next, &a);
        /* p *= 2 */
        STP_Number_mul(&p, &two);
    }

    /* (a+b)^2 */
    STP_Number_add(&a, &b);
    STP_Number_sqr(&a);
    _STP_Number_rough_round(&a, wp + 10);

    /* t*4 */
    STP_Number_set(&two, 4);
    STP_Number_mul(&t, &two);
    /* (a+b)^2 / t*4 */
    STP_Number_div(&a, &t, wp);

    *out = a;
    STP_Number_destroy(&b);
    STP_Number_destroy(&t);
    STP_Number_destroy(&p);
    STP_Number_destroy(&two);
    STP_Number_destroy(&a_next);
    return 1;
}
