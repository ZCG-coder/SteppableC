#pragma once

#include "stp_number.h"

#include <stdint.h>

/* add */
int _STP_Number_add(STP_Number* num, uint64_t rhs);
int _STP_Number_add_abs(STP_Number* lhs, const STP_Number* rhs);
uint64_t _STP_add64_carry(uint64_t* acc, uint64_t value);

/* sub */
int _STP_Number_sub_abs(STP_Number* lhs, const STP_Number* rhs);

/* cmp */
int STP_Number_cmp_abs(const STP_Number* lhs, const STP_Number* rhs);

/* scale */
int _STP_Number_mul(STP_Number* num, uint64_t fac);
int _STP_Number_mul_exp(STP_Number* num, uint64_t diff_scale);
uint8_t _STP_Number_mod10(STP_Number* num);

/* alloc */
int _STP_Number_align_scales(STP_Number* lhs, STP_Number* rhs);
int _STP_Number_trim(STP_Number* num);
int _STP_Number_slice(STP_Number* dst, const STP_Number* src, uint64_t start, uint64_t len);

int _STP_Number_ensure_capacity(STP_Number* num, uint64_t min_capacity);

/* shift */
int _STP_Number_lshift_blocks(STP_Number* num, uint64_t blocks);
