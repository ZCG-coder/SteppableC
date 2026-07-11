#pragma once

#include "stp_number.h"

#include <stdint.h>

int _STP_Number_add(STP_Number* num, uint64_t rhs);
int _STP_Number_mul(STP_Number* num, uint64_t fac);
int _STP_Number_mul_exp(STP_Number* num, uint64_t diff_scale);

uint8_t _STP_Number_mod10(STP_Number* num);

int _STP_Number_trim(STP_Number* num);

int _STP_Number_ensure_capacity(STP_Number* num, uint64_t min_capacity);
