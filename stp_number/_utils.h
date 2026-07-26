#pragma once

#include "stp_number.h"

#include <stdint.h>

static const uint64_t _EXPS[] = {
    /* 10^0  = */ 1ULL,
    /* 10^1  = */ 10ULL,
    /* 10^2  = */ 100ULL,
    /* 10^4  = */ 10000ULL,
    /* 10^8  = */ 100000000ULL,
    /* 10^16 = */ 10000000000000000ULL
};

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
 * NOTE sign and scale are not used
 * MODIFIES size and arr of lhs
 */
int _STP_Number_add_abs(STP_Number* lhs, const STP_Number* rhs);

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
 * NOTE sign and scale are not used
 * MODIFIES size and arr of lhs
 */
int _STP_Number_sub_abs(STP_Number* lhs, const STP_Number* rhs);

/* cmp */
/**
 * Compare absolute magnitude of rhs and lhs
 *
 * ARG lhs
 * ARG rhs
 * RETURNS 1 if lhs > rhs, 0 if lhs == rhs, -1 if lhs < rhs.
 *
 * REQUIRES lhs and rhs are not NULL.
 */
int _STP_Number_cmp_abs(const STP_Number* lhs, const STP_Number* rhs);

/* scale */

/**
 * Quickly multiply num be an exponent of 10, i.e., 10^diff_scale.
 *
 * ARG num
 * ARG diff_scale
 * RETURNS 1 if successful, 0 if not.
 *
 * REQUIRES num is not NULL
 * MODIFIES arr of num
 */
int _STP_Number_mul_exp(STP_Number* num, uint64_t diff_scale);

/* alloc */
/**
 * Align scales of lhs and rhs.
 *
 * ARG lhs
 * ARG rhs
 *
 * RETURNS 1 if successful, 0 otherwise
 * REQUIRES lhs and rhs are not NULL
 *
 * MODIFIES lhs and rhs scales (sets them to become equal), lhs and rhs arr (multiplies exponent of 10 to them)
 */
int _STP_Number_align_scales(STP_Number* lhs, STP_Number* rhs);

/**
 * Trims leading zero blocks from num
 *
 * ARG num
 *
 * RETURNS 1 if successful, 0 otherwise
 * MODIFIES shrinks size of num
 */
int _STP_Number_trim(STP_Number* num);

/**
 * Copy a number of blocks from src to dst.
 *
 * ARG dst destination to write blocks to
 * ARG src source
 * ARG start start index of block
 * ARG len number of blocks to copy
 */
int _STP_Number_slice(STP_Number* dst, const STP_Number* src, uint64_t start, uint64_t len);

/**
 * Ensure the number has capacity of at least min_capacity.
 *
 * ARG num
 * ARG min_capacity
 * RETURNS 1 if resize is successful
 *
 * MODIFIES capacity and arr of num.
 * NOTE newly allocated memory is automatically zero-filled.
 */
int _STP_Number_ensure_capacity(STP_Number* num, uint64_t min_capacity);

#define _STP_PRINT_NUM(n, L)             \
    do                                   \
    {                                    \
        STP_String str;                  \
        STP_String_init(&str);           \
        STP_Number_print(&n, &str);      \
        printf("%s = %s\n", L, str.str); \
        STP_String_destroy(&str);        \
    } while (0)
