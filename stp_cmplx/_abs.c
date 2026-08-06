#include "stp_cmplx.h"
#include "stp_number.h"

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

int STP_Cmplx_abs(STP_Cmplx* const cmplx, STP_Number* out, int64_t wp)
{
    if (cmplx == NULL)
        return 0;
    if (cmplx->real == NULL || cmplx->imag == NULL)
        return 0;

    /* if cannot write to out, write to cmplx real */
    if (out == NULL)
        out = cmplx->real;

    int status = 1;

    STP_Cmplx tmp;
    status &= STP_Cmplx_init(&tmp);
    status &= STP_Cmplx_copy(cmplx, &tmp);

    /* |z| = sqrt(a^2 + b^2) */
    status &= STP_Number_sqr(tmp.real);
    status &= STP_Number_sqr(tmp.imag);
    status &= STP_Number_add(tmp.real, tmp.imag);
    status &= STP_Number_sqrt(tmp.real, wp);

    free(out->arr);
    *out = *tmp.real;
    status &= STP_Number_destroy(tmp.imag);

    return status;
}
