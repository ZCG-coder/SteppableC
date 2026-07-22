#pragma once

#include "stp_number.h"

#include <stdint.h>

/* add */
/**
 * Add a 64-bit value to num.
 *
 * ARG num lhs
 * ARG rhs value to be added
 * RETURNS 1 if successful, 0 if not
 *
 * REQUIRES num is not null
 *
 * NOTE a new block is created automatically in case of carry spill
 */
int _STP_Number_add(STP_Number* num, uint64_t rhs);

/**
 * Add rhs to lhs without considering scales.
 *
 * ARG lhs
 * ARG rhs
 * RETURNS 1 if successful, 0 if not
 *
 * REQUIRES lhs and rhs are not null
 *
 * NOTE scale is not used
 * MODIFIES size and arr of lhs
 */
int _STP_Number_add_abs(STP_Number* lhs, const STP_Number* rhs);

/**
 * Add value to acc, returning the carry
 *
 * ARG acc pointer to lhs
 * ARG value rhs to be added to lhs
 * RETURNS carry after add
 *
 * NOTE This does not null-check acc.
 */
uint64_t _STP_add64_carry(uint64_t* acc, uint64_t value);

/* sub */
/**
 * Subtracts rhs from lhs without considering scales.
 *
 * ARG lhs
 * ARG rhs
 * RETURNS 1 if successful, 0 if not
 *
 * REQUIRES lhs and rhs are not null
 * REQUIRES lhs >= rhs
 *
 * NOTE scale is not used
 * MODIFIES size and arr of lhs
 */
int _STP_Number_sub_abs(STP_Number* lhs, const STP_Number* rhs);

/* mul */
void _STP_mul64_wide(uint64_t lhs, uint64_t rhs, uint64_t* high, uint64_t* low);

/* cmp */
int _STP_Number_cmp_abs(const STP_Number* lhs, const STP_Number* rhs);

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

#define _STP_PRINT_NUM(n, L)             \
    do                                   \
    {                                    \
        STP_String str;                  \
        STP_String_init(&str);           \
        STP_Number_print(&n, &str);      \
        printf("%s = %s\n", L, str.str); \
        STP_String_destroy(&str);        \
    } while (0)
