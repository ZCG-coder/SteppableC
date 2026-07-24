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
 * NOTE sign and scale are not used
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
 * NOTE sign and scale are not used
 * MODIFIES size and arr of lhs
 */
int _STP_Number_sub_abs(STP_Number* lhs, const STP_Number* rhs);

/* mul */
/**
 * Multiply lhs and rhs, returning 128-bit number in high and low halves.
 *
 * ARG lhs
 * ARG rhs
 *
 * RETURNS carry after multiplication
 * REQUIRES high and low are not NULL.
 */
void _STP_mul64_wide(uint64_t lhs, uint64_t rhs, uint64_t* high, uint64_t* low);

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
 * Multiply a number by a factor
 *
 * ARG num
 * ARG fac factor
 * RETURNS 1 if successful, 0 otherwise.
 *
 * REQUIRES num is not NULL
 */
int _STP_Number_mul(STP_Number* num, uint64_t fac);

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

void _STP_Number_div_uint32(STP_Number* num, uint32_t divisor);

/**
 * Divide a number by 10 and return the remainder (0-9 inclusive)
 *
 * ARG num
 * RETURNS remainder of the division (0-9 inclusive)
 *
 * REQUIRES num is not NULL
 * MODIFIES arr of num. Stores quotient of division in num.
 */
uint8_t _STP_Number_mod10(STP_Number* num);

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

/**
 * Count number of occupied bits in num
 *
 * ARG num
 * RETURNS number of occupied bits
 * REQUIRES num is not NULL.
 */
uint64_t _STP_Number_bit_count(STP_Number* num);

/* shift */
/**
 * Left-shift the number by blocks of 64-bit blocks.
 *
 * ARG num
 * ARG blocks number of blocks to left-shift
 * RETURNS 1 if successful, 0 otherwise
 *
 * NOTE carry is handled automatically
 * REQUIRES num is not NULL
 */
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
