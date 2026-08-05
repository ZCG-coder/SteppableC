#include "_div.h"
#include "_utils.h"
#include "helpers.h"
#include "stp_number.h"

#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

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

    int8_t final_sign = (lhs->sign == rhs->sign) ? 1 : -1;

    /* initial guess */
    uint64_t top = rhs->arr[rhs->size - 1];
    uint64_t d_top = _count_digits(top);
    long double q_norm = (long double)top * powl(10, -(long double)d_top);
    if (rhs->size > 1)
        q_norm += (long double)(rhs->arr[rhs->size - 2]) * powl(10, -((long double)d_top + 19));

    long double x_norm = 1.0L / q_norm;
    long double M_x0 = x_norm * 1000000000000000000.0L; /* 10^19 */
    uint64_t M_u = (uint64_t)M_x0;
    if (M_u >= _BASE_10_19)
        M_u = _BASE_10_19 - 1;

    int64_t S_Q = _STP_Number_sig_digits(rhs);
    int64_t S_x0 = -(18 + S_Q + rhs->scale);

    STP_Number two, T1, T2, x0;
    STP_Number_init(&two);
    STP_Number_init(&T1);
    STP_Number_init(&T2);
    STP_Number_init(&x0);

    STP_Number_set(&x0, M_u);
    x0.scale = S_x0;
    x0.sign = 1;

    int64_t log_P = _STP_Number_int_digits(lhs);
    int64_t log_Q = _STP_Number_int_digits(rhs);

    int64_t S_req = decimal_places + 1 + log_P - log_Q;
    if (S_req < 0)
        S_req = 0;

    uint64_t n_L = 0;
    if (S_req > 15)
        n_L = (uint64_t)ceill(log2l((long double)S_req / 15.0L));

    int64_t guard_places = decimal_places + log_P + 10;
    if (guard_places < 10)
        guard_places = 10;

    for (uint64_t j = 0; j < n_L; ++j)
    {
        /* T1 = rhs * x0 */
        STP_Number_copy(rhs, &T1);
        T1.sign = 1;
        STP_Number_mul(&T1, &x0);
        STP_Number_round(&T1, guard_places);

        STP_Number_set(&two, 2);

        /* T2 = 2 - T1 */
        STP_Number_copy(&two, &T2);
        STP_Number_sub(&T2, &T1);

        /* x0 = x0 * T2 */
        STP_Number_mul(&x0, &T2);
        STP_Number_round(&x0, guard_places);
    }

    STP_Number_mul(lhs, &x0);
    STP_Number_round(lhs, decimal_places);
    lhs->sign = final_sign;

    STP_Number_destroy(&two);
    STP_Number_destroy(&T1);
    STP_Number_destroy(&T2);
    STP_Number_destroy(&x0);
    return 1;
}
#endif
