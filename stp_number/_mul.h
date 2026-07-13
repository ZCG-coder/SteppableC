#pragma once

#include "stp_number.h"

#define STP_MUL_KARATSUBA_THRESHOLD 32ULL

int _STP_Number_mul_abs_schoolbook(STP_Number* out, const STP_Number* lhs, const STP_Number* rhs);

int _STP_Number_mul_abs_karatsuba(STP_Number* out, const STP_Number* lhs, const STP_Number* rhs);
