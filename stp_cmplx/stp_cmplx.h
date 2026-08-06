#pragma once

#include "stp_number.h"
#include "stp_string.h"

#include <stdint.h>

/**
 * Complex number object
 *
 * FLD real pointer to real part
 * FLD imag pointer to imaginary part
 */
typedef struct
{
    STP_Number* real;
    STP_Number* imag;
} STP_Cmplx;

/**
 * Initializes cmplx
 *
 * ARG cmplx
 * RETURNS 1 if successful
 */
int STP_Cmplx_init(STP_Cmplx* cmplx);

/**
 * Initializes cmplx from real and imag strings.
 *
 * ARG cmplx
 * ARG real
 * ARG imag
 * RETURNS 1 if successful, 0 otherwise.
 *
 * REQUIRES real and imag are NULL-terminated.
 */
int STP_Cmplx_conv(STP_Cmplx* cmplx, char* const real, char* const imag);

/**
 * Copy lhs to rhs.
 *
 * ARG lhs
 * ARG rhs
 * RETURNS 1 if successfully copied, 0 if not.
 *
 * REQUIRES lhs and rhs are initialized and non-NULL.
 */
int STP_Cmplx_copy(STP_Cmplx* const lhs, STP_Cmplx* rhs);

/**
 * Destroys cmplx object.
 *
 * ARG cmplx
 * RETURNS 1 if successful, 0 if not.
 *
 * REQUIRES cmplx is initialized and non-NULL.
 */
int STP_Cmplx_destroy(STP_Cmplx* cmplx);

/**
 * Prints a cmplx object to out.
 *
 * ARG cmplx
 * ARG out Output string
 * RETURNS 1 if successful, 0 if not.
 *
 * REQUIRES cmplx is initialized and non-NULL.
 * REQUIRES out is initialized and non-NULL.
 */
int STP_Cmplx_print(STP_Cmplx* cmplx, STP_String* out);

/**
 * Tests if lhs and rhs are equal in value.
 *
 * ARG lhs
 * ARG rhs
 * RETURNS 1 if equal, 0 if not.
 *
 * REQUIRES lhs and rhs are initialized and non-NULL.
 */
int STP_Cmplx_eq(STP_Cmplx* lhs, STP_Cmplx* rhs);

/* arithmetic */

/**
 * Adds rhs to lhs, stores result in lhs.
 *
 * ARG lhs
 * ARG rhs
 * RETURNS 1 if successful, 0 if not.
 *
 * REQUIRES lhs and rhs are initialized and non-NULL.
 */
int STP_Cmplx_add(STP_Cmplx* lhs, STP_Cmplx* rhs);

/**
 * Subtracts rhs from lhs, stores result in lhs.
 *
 * ARG lhs
 * ARG rhs
 * RETURNS 1 if successful, 0 if not.
 *
 * REQUIRES lhs and rhs are initialized and non-NULL.
 */
int STP_Cmplx_sub(STP_Cmplx* lhs, STP_Cmplx* rhs);

/**
 * Multiplies rhs to lhs, stores result in lhs.
 *
 * ARG lhs
 * ARG rhs
 * RETURNS 1 if successful, 0 if not.
 *
 * REQUIRES lhs and rhs are initialized and non-NULL.
 */
int STP_Cmplx_mul(STP_Cmplx* lhs, STP_Cmplx* rhs);

/**
 * Divide lhs to rhs, stores result in lhs.
 *
 * ARG lhs
 * ARG rhs
 * ARG wp decimal digits to be precise to.
 * RETURNS 1 if successful, 0 if not.
 *
 * REQUIRES lhs and rhs are initialized and non-NULL.
 * REQUIRES wp > 10. If lower value is given, 10 is used.
 */
int STP_Cmplx_div(STP_Cmplx* lhs, STP_Cmplx* rhs, int64_t wp);

/* abs / magnitude */

/**
 * Gets magnitude |z| of cmplx.
 *
 * ARG cmplx
 * ARG out output number. If NULL, outputs to cmplx->real.
 * ARG wp decimal digits to be correct to.
 * RETURNS 1 if successful, 0 if not.
 *
 * REQUIRES cmplx is initialized and non-NULL.
 * REQUIRES wp > 10. If not, 10 is used.
 * NOTE if out is NULL, writes result to cmplx->real.
 */
int STP_Cmplx_abs(STP_Cmplx* cmplx, STP_Number* out, int64_t wp);

/* conjg */

/**
 * Gets conjugate of cmplx, stores result in cmplx.
 *
 * ARG cmplx
 * RETURNS 1 if successful, 0 if not.
 *
 * REQUIRES cmplx is initialized and non-NULL.
 */
int STP_Cmplx_conjg(STP_Cmplx* cmplx);
