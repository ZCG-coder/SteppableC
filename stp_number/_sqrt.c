#include "_utils.h"
#include "helpers.h"
#include "stp_number.h"

#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int STP_Number_sqrt(STP_Number* x, uint64_t wp)
{
    if (x == NULL)
        return 0;
    if (x->sign < 0)
    {
        fprintf(stderr, "%s: cannot take sqrt of negative number", STP_CURRENT_FUNCTION);
        return 0;
    }

    if (STP_Number_is_zero(x))
        return 1;

    _STP_Number_trim(x);

    int64_t scale_shift = (x->scale >= 0) ? (x->scale / 2) : ((x->scale - 1) / 2);
    int64_t remainder = x->scale - (scale_shift * 2);
    long double scale_adj = (remainder == 1) ? 10.0L : 1.0L;

    long double T = 0.0L;
    int64_t k = 0;
    uint64_t N = x->size;
    char buf[64];

    if (N == 1)
    {
        T = (long double)x->arr[0];
        T = sqrtl(T * scale_adj);
        k = 0;
    }
    else
    {
        T = (long double)x->arr[N - 1] * 1.0E19L + (long double)x->arr[N - 2];
        T *= scale_adj;

        k = ((int64_t)N - 2) / 2;

        /* N is odd */
        if (N % 2 == 1)
            T = sqrtl(T * 1.0E19L);
        /* N is even */
        else
            T = sqrtl(T);
    }

    sprintf(buf, "%Lf", T);

    STP_Number x0;
    STP_Number_conv(&x0, buf);
    x0.scale += (k * 19) + scale_shift;

    STP_Number two, S;
    STP_Number_conv(&two, "2");
    STP_Number_init(&S);

    int64_t log_X = _STP_Number_int_digits(x);
    int64_t S_req = wp + 1 + log_X / 2;
    if (S_req < 0)
        S_req = 0;

    uint64_t n_L = 0;
    if (S_req > 15)
        n_L = (uint64_t)ceill(log2l((long double)S_req / 15.0L));

    int64_t guard_places = wp + log_X / 2 + 10;
    if (guard_places < 10)
        guard_places = 10;

    for (uint64_t j = 0; j < n_L; ++j)
    {
        /* T1 = S/x0 */
        STP_Number_copy(x, &S);
        STP_Number_div(&S, &x0, guard_places);

        /* T1 = x0 + T1 */
        STP_Number_add(&x0, &S);

        /* T1 = 1/2 T1 */
        STP_Number_div(&x0, &two, guard_places);
    }

    STP_Number_round(&x0, wp);
    free(x->arr);
    *x = x0;

    STP_Number_destroy(&two);
    return 1;
}
