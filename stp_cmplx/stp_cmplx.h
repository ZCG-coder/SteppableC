#pragma once

#include "stp_number.h"
#include "stp_string.h"

typedef struct
{
    STP_Number* real;
    STP_Number* imag;
} STP_Cmplx;

int STP_Cmplx_init(STP_Cmplx* cmplx);

int STP_Cmplx_copy(STP_Cmplx* const lhs, STP_Cmplx* rhs);

int STP_Cmplx_destroy(STP_Cmplx* cmplx);

int STP_Cmplx_print(STP_Cmplx* cmplx, STP_String* out);