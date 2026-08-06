#include "stp_number.h"

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
    STP_Number_div(&x, num_inv, wp + 4);

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
    STP_Number_round(&x, wp + 4);

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
        STP_Number_div(&tmp, &denom, wp + 4);
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
    STP_Number i1;
    STP_Number i2;
    STP_Number_conv(&i1, "5");
    STP_Number_conv(&i2, "239");

    /* log10(5) = 0.698970004336019 */
    double iter_5ld = (double)wp / 0.698970004336019L;
    iter_5ld -= 1.0L;
    iter_5ld /= 2.0L;
    iter_5ld += 2.0L;

    /* log10(239) = 2.37839790094814 */
    double iter_239ld = (double)wp / 2.37839790094814L;
    iter_239ld -= 1.0L;
    iter_239ld /= 2.0L;
    iter_239ld += 2.0L;

    STP_Number mul1, mul2;
    STP_Number_conv(&mul1, "16");
    STP_Number_conv(&mul2, "4");

    /* atan(1/5) */
    _atan(&i1, (uint64_t)iter_5ld + 1, wp + 4);
    /* atan(1/239) */
    _atan(&i2, (uint64_t)iter_239ld + 1, wp + 4);

    /* 16*atan(1/5) */
    STP_Number_mul(&i1, &mul1);
    /* 4*atan(1/239) */
    STP_Number_mul(&i2, &mul2);

    /* 16*atan(1/5) - 4*atan(1/239) = pi */
    STP_Number_sub(&i1, &i2);
    STP_Number_round(&i1, wp);

    *out = i1;

    STP_Number_destroy(&i2);
    STP_Number_destroy(&mul1);
    STP_Number_destroy(&mul2);
    return 1;
}
