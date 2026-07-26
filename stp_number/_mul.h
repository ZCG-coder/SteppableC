#pragma once

#include "stp_number.h"

#define STP_MUL_KARATSUBA_THRESHOLD 32ULL

void _mul_add(uint64_t A, uint64_t B, uint64_t C, uint64_t K, uint64_t* out_digit, uint64_t* out_carry);

int _STP_Number_mul_abs_schoolbook(STP_Number* out, const STP_Number* lhs, const STP_Number* rhs);

int _STP_Number_mul_abs_karatsuba(STP_Number* out, const STP_Number* lhs, const STP_Number* rhs);
