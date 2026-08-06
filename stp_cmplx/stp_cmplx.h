#pragma once

#include "stp_number.h"
#include "stp_string.h"

#include <stdint.h>

typedef struct
{
    STP_Number* real;
    STP_Number* imag;
} STP_Cmplx;

int STP_Cmplx_init(STP_Cmplx* cmplx);

int STP_Cmplx_conv(STP_Cmplx* cmplx, char* const real, char* const imag);

int STP_Cmplx_copy(STP_Cmplx* const lhs, STP_Cmplx* rhs);

int STP_Cmplx_destroy(STP_Cmplx* cmplx);

int STP_Cmplx_print(STP_Cmplx* cmplx, STP_String* out);

int STP_Cmplx_eq(STP_Cmplx* lhs, STP_Cmplx* rhs);

/* arithmetic */
int STP_Cmplx_add(STP_Cmplx* lhs, STP_Cmplx* rhs);

int STP_Cmplx_sub(STP_Cmplx* lhs, STP_Cmplx* rhs);

int STP_Cmplx_mul(STP_Cmplx* lhs, STP_Cmplx* rhs);

int STP_Cmplx_div(STP_Cmplx* lhs, STP_Cmplx* rhs, int64_t wp);

/* abs / magnitude */
int STP_Cmplx_abs(STP_Cmplx* cmplx, STP_Number* out, int64_t wp);

/* conjg */
int STP_Cmplx_conjg(STP_Cmplx* cmplx);
