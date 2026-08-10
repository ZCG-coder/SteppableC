#include "stp_number.h"

#include <math.h>
#include <stdint.h>
#include <string.h>

uint64_t _sin_loops(int64_t wp)
{
    double K = (double)wp + 4.0;

    /* C = (K * ln(10)) / (e * pi) */
    double C = 0.26963102436 * K;
    if (C < 3.0)
        C = 3.0;

    double lnC = log(C);

    /* ln(u) = C */
    double u_ub = C / (lnC - log(lnC));
    double n_est = 4.269867 * u_ub + 7.5;

    return (uint64_t)ceil(n_est);
}

int STP_Number_sin(STP_Number* num, int64_t wp)
{
    if (wp < 0)
        return 0;
    if (num == NULL || num->arr == NULL)
        return 0;

    /* range reduction */
    STP_Number pi;
    STP_Number_pi(&pi, wp);

    STP_Number_destroy(&pi);
    return 1;
}
