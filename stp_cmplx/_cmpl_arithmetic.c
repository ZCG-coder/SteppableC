#include "_utils.h"
#include "stp_cmplx.h"
#include "stp_number.h"

#include <stdint.h>
#include <stdlib.h>

int STP_Cmplx_eq(STP_Cmplx* lhs, STP_Cmplx* rhs)
{
    if (lhs == NULL || rhs == NULL)
        return 0;

    return (STP_Number_cmp(lhs->real, rhs->real) == 0) && (STP_Number_cmp(lhs->imag, rhs->imag) == 0);
}

int STP_Cmplx_add(STP_Cmplx* lhs, STP_Cmplx* rhs)
{
    if (lhs == NULL || rhs == NULL)
        return 0;

    int status = 1;
    status &= STP_Number_add(lhs->real, rhs->real);
    status &= STP_Number_add(lhs->imag, rhs->imag);

    return status;
}

int STP_Cmplx_sub(STP_Cmplx* lhs, STP_Cmplx* rhs)
{
    if (lhs == NULL || rhs == NULL)
        return 0;

    int status = 1;
    status &= STP_Number_sub(lhs->real, rhs->real);
    status &= STP_Number_sub(lhs->imag, rhs->imag);

    return status;
}

int STP_Cmplx_mul(STP_Cmplx* lhs, STP_Cmplx* rhs)
{
    if (lhs == NULL || rhs == NULL)
        return 0;

    /*
     *   (a + i*b) * (c + i*d)
     * = (ac - bd) + i*(ad + bc)
     */
    int status = 1;
    STP_Number l_real;
    STP_Number l_imag;
    STP_Number_init(&l_real);
    STP_Number_init(&l_imag);

    STP_Number_copy(lhs->real, &l_real);
    STP_Number_copy(lhs->imag, &l_imag);

    STP_Number_mul(&l_real, rhs->imag); /* ad */
    STP_Number_mul(&l_imag, rhs->real); /* bc */
    STP_Number_add(&l_real, &l_imag); /* ad + bc */

    STP_Number new_imag;
    STP_Number_init(&new_imag);
    STP_Number_copy(&l_real, &new_imag);

    STP_Number_copy(lhs->real, &l_real);
    STP_Number_copy(lhs->imag, &l_imag);

    STP_Number_mul(&l_real, rhs->real); /* ac */
    STP_Number_mul(&l_imag, rhs->imag); /* bd */
    STP_Number_sub(&l_real, &l_imag); /* ac - bd */

    free(lhs->real->arr);
    free(lhs->imag->arr);
    *lhs->real = l_real;
    *lhs->imag = new_imag;

    STP_Number_destroy(&l_imag);
    return status;
}

int STP_Cmplx_div(STP_Cmplx* lhs, STP_Cmplx* rhs, int64_t wp)
{
    if (lhs == NULL || rhs == NULL)
        return 0;

    int status = 1;

    /*
     * 1 / (c + id) = c / (c^2 + d^2) - id / (c^2 + d^2)
     */
    STP_Number denom;
    STP_Number c;
    STP_Number d;
    status &= STP_Number_init(&denom);
    status &= STP_Number_copy(rhs->real, &denom);
    status &= STP_Number_sqr(&denom); /* c^2 */

    status &= STP_Number_init(&c);
    status &= STP_Number_copy(rhs->real, &c);
    status &= STP_Number_init(&d);
    status &= STP_Number_copy(rhs->imag, &d);

    status &= STP_Number_sqr(&d); /* d^2 */
    status &= STP_Number_add(&denom, &d); /* c^2 + d^2 */

    /* calculate wp */
    int64_t D_a = _STP_Number_int_digits(lhs->real);
    int64_t D_b = _STP_Number_int_digits(lhs->imag);
    int64_t D_max = (D_a > D_b) ? D_a : D_b;
    D_max++;

    status &= STP_Number_copy(rhs->imag, &d);
    status &= STP_Number_div(&c, &denom, D_max + wp); /* c / (c^2 + d^2) */
    status &= STP_Number_div(&d, &denom, D_max + wp); /* -d / (c^2 + d^2) */
    d.sign = -d.sign;

    STP_Cmplx new_rhs;
    new_rhs.real = &c;
    new_rhs.imag = &d;
    status &= STP_Cmplx_mul(lhs, &new_rhs);

    /* rounding */
    status &= STP_Number_round(lhs->real, wp);
    status &= STP_Number_round(lhs->imag, wp);

    status &= STP_Number_destroy(&c);
    status &= STP_Number_destroy(&d);
    status &= STP_Number_destroy(&denom);

    return status;
}
