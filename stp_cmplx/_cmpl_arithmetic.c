#include "stp_number.h"
#include "stp_cmplx.h"

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
    *   (a + ib) * (c + id)
    * = (ac - bd) + i(ad + bc)
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
    return 1;
}