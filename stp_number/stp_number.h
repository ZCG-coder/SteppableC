#pragma once

#include "stp_string.h"

#include <stdint.h>

#define _STP_NUMBER_DEFAULT_CAPACITY 4
#define DECIMAL_SEP '.'

/**
 * Number object
 *
 * FLD size Number of blocks used
 * FLD capacity Number of blocks allocated
 * FLD arr Array of blocks
 *
 * FLD scale Exponent n, multiplies 10^n to number
 * FLD sign Sign of number. +1 if positive, -1 if negative; 0 if zero (not enforced).
 */
typedef struct
{
    uint64_t size;
    uint64_t capacity;
    uint64_t* arr;

    int64_t scale;
    int8_t sign;
} STP_Number;

/**
 * Initializes a STP_Number instance.
 *
 * ARG num pointer to unitialized number
 * RETURNS 1 always.
 */
int STP_Number_init(STP_Number* num);

/**
 * Creates a STP_Number from string representation
 *
 * ARG num pointer to unitialized number
 * ARG from string of digits/decimal point...
 * RETURNS 1 if successful. 0 otherwise
 *
 * NOTE Decimal separator is defined in stp_number.h. Ensure decimal separator matches.
 */
int STP_Number_conv(STP_Number* num, const char* from);

/**
 * Copies memory from num to rhs.
 *
 * ARG num pointer to initialized number
 * ARG rhs destination to copy to
 * RETURNS 1 if successfully copied
 *
 * NOTE capacity and properties are handled successfully
 * REQUIRES rhs is initialized already
 */
int STP_Number_copy(const STP_Number* num, STP_Number* rhs);

/**
 * Frees up memory by destroying num.
 *
 * ARG num number to be destroyed
 * RETURNS 1 if successfully destroyed
 *
 * REQUIRES num is already initialized.
 */
int STP_Number_destroy(STP_Number* num);

/**
 * Clears all blocks from the number, sets its value to zero.
 *
 * ARG num
 * RETURNS 1 if successful, 0 otherwise
 *
 * MODIFIES size and capacity of num. first element of arr is set to zero.
 * REQUIRES num is not NULL.
 */
int STP_Number_clear(STP_Number* num);

/**
 * Sets STP_Number to 1.
 *
 * ARG num
 * RETURNS 1 if successful, 0 otherwise
 *
 * REQUIRES num is not NULL
 */
int STP_Number_to_one(STP_Number* num);

/**
 * Print out num in decimal to out.
 *
 * ARG num
 * ARG out output string
 *
 * RETURNS 1 if successful, 0 otherwise.
 * REQUIRES num is not NULL
 * NOTE out can be a non-initialized string.
 */
int STP_Number_print(const STP_Number* num, STP_String* out);

/*
 * Arithmetic operations
 */
/**
 * Checks if the number is zero
 *
 * ARG num
 * RETURNS 1 if number is zero, 0 if not, another arbitrary is returned if an error occured.
 *
 * REQUIRES num is not NULL.
 */
int STP_Number_is_zero(const STP_Number* num);

/**
 * Compares two numbers
 *
 * ARG lhs
 * ARG rhs
 * RETURNS 1 if lhs > rhs, 0 if lhs == rhs, -1 if lhs < rhs.
 *
 * REQUIRES lhs and rhs are not NULL.
 * NOTE scales of lhs and rhs are considered.
 */
int STP_Number_cmp(STP_Number* lhs, STP_Number* rhs);

/**
 * Add two numbers together. Result stored in lhs.
 *
 * ARG lhs
 * ARG _rhs
 * RETURNS 1 if successful, 0 if not
 *
 * REQUIRES lhs and rhs are not null
 *
 * NOTE carry is handled automatically
 *
 * MODIFIES capacity, size and arr of lhs
 */
int STP_Number_add(STP_Number* lhs, STP_Number* _rhs);

/**
 * Subtracts rhs from lhs. Result stored in lhs.
 *
 * ARG lhs
 * ARG rhs
 * RETURNS 1 if successful, 0 if not
 *
 * REQUIRES lhs and rhs are not null
 */
int STP_Number_sub(STP_Number* lhs, STP_Number* rhs);

/**
 * Multiplies lhs and rhs. Result stored in lhs.
 *
 * ARG lhs
 * ARG rhs
 * RETURNS 1 if successful, 0 if not
 *
 * REQUIRES lhs and rhs are not NULL
 */
int STP_Number_mul(STP_Number* lhs, STP_Number* rhs);

/**
 * Squares num. Result stored in num.
 *
 * ARG num
 * RETURNS 1 if successful, 0 otherwise.
 * REQUIRES num is valid and initialized.
 */
int STP_Number_sqr(STP_Number* num);

/**
 * Divides lhs by rhs. Result stored in lhs.
 *
 * ARG lhs
 * ARG rhs
 * ARG decimal_places number of decimal places that result is accurate to.
 * RETURNS 1 if successful, 0 otherwise.
 *
 * REQUIRES lhs and rhs are not null
 * MODIFIES scale of lhs.
 */
int STP_Number_div(STP_Number* lhs, const STP_Number* rhs, uint64_t decimal_places);

/**
 * Left-shifts a number by bits. Result stored in num.
 *
 * ARG num
 * ARG bits
 * RETURNS 1 if successful, 0 otherwise.
 *
 * REQUIRES num is not NULL.
 * NOTE carry handled automatically
 */
int STP_Number_lshift(STP_Number* num, uint64_t bits);

/**
 * Right-shifts a number by bits. Result stored in num.
 *
 * ARG num
 * ARG bits
 * RETURNS 1 if successful, 0 otherwise.
 *
 * REQUIRES num is not NULL.
 */
int STP_Number_rshift(STP_Number* num, uint64_t bits);

/* abs */
/**
 * Takes absolute value
 *
 * ARG num
 * RETURNS 1 if successful, 0 otherwise.
 * REQUIRES num is not NULL.
 *
 * MODIFIES sign of num
 */
int STP_Number_abs(STP_Number* num);
