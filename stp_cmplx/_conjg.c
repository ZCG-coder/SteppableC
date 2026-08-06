#include "stp_cmplx.h"

#include <string.h>

int STP_Cmplx_conjg(STP_Cmplx* cmplx)
{
    if (cmplx == NULL)
        return 0;
    if (cmplx->real == NULL || cmplx->imag == NULL)
        return 0;

    cmplx->imag->sign = -cmplx->imag->sign;
    return 1;
}
