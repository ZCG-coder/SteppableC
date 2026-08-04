#include "stp_cmplx.h"

#include <stdlib.h>

int STP_Cmplx_init(STP_Cmplx* cmplx)
{
    if (cmplx == NULL)
        return 0;
    if (cmplx->real == NULL || cmplx->imag == NULL)
        return 0;

    int status = 1;
    cmplx->real = malloc(1 * sizeof(STP_Number));
    cmplx->imag = malloc(1 * sizeof(STP_Number));

    status &= STP_Number_init(cmplx->real);
    status &= STP_Number_init(cmplx->imag);
    return status;
}

int STP_Cmplx_conv(STP_Cmplx* cmplx, char* const real, char* const imag)
{
    if (cmplx == NULL)
        return 0;
    if (real == NULL || imag == NULL)
        return 0;

    cmplx->real = malloc(1 * sizeof(STP_Number));
    cmplx->imag = malloc(1 * sizeof(STP_Number));

    STP_Number_conv(cmplx->real, real);
    STP_Number_conv(cmplx->imag, imag);
    return 1;
}

int STP_Cmplx_copy(STP_Cmplx* const lhs, STP_Cmplx* rhs)
{
    if (lhs == NULL || rhs == NULL)
        return 0;
    if (lhs->real == NULL || lhs->imag == NULL)
        return 0;
    if (rhs->real == NULL || rhs->imag == NULL)
        return 0;

    int status = 1;
    status &= STP_Number_copy(lhs->real, rhs->real);
    status &= STP_Number_copy(lhs->imag, rhs->imag);
    return status;
}

int STP_Cmplx_destroy(STP_Cmplx* cmplx)
{
    if (cmplx == NULL)
        return 0;
    if (cmplx->real == NULL || cmplx->imag == NULL)
        return 0;

    int status = 1;
    status &= STP_Number_destroy(cmplx->real);
    status &= STP_Number_destroy(cmplx->imag);

    free(cmplx->real);
    free(cmplx->imag);
    cmplx->real = NULL;
    cmplx->imag = NULL;
    return status;
}

int STP_Cmplx_print(STP_Cmplx* cmplx, STP_String* out)
{
    if (cmplx == NULL || out == NULL)
        return 0;
    if (cmplx->real == NULL || cmplx->imag == NULL)
        return 0;

    STP_String real_str;
    STP_String imag_str;
    STP_String_init(&real_str);
    STP_String_init(&imag_str);

    STP_String plus_i_times = STP_String_lit(" + i ");

    /*
    format:
    real + i imag
    */
    STP_Number_print(cmplx->real, &real_str);
    STP_Number_print(cmplx->imag, &imag_str);

    STP_String_append(&real_str, &plus_i_times);
    STP_String_append(&real_str, &imag_str);

    free(out->str);
    *out = real_str;
    STP_String_destroy(&imag_str);
    STP_String_destroy(&plus_i_times);
    return 1;
}
