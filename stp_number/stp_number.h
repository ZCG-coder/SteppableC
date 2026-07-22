#pragma once

#include "stp_string.h"

#include <stdint.h>

#define _STP_NUMBER_DEFAULT_CAPACITY 4
#define DECIMAL_SEP '.'

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
int STP_Number_clear(STP_Number* num);

int STP_Number_print(const STP_Number* num, STP_String* out);

/*
 * Arithmetic operations
 */

int STP_Number_is_zero(const STP_Number* num);
int STP_Number_cmp(STP_Number* lhs, STP_Number* rhs);

/**
 * Add two numbers together.
 *
 * ARG lhs
 * ARG _rhs
 * RETURNS 1 if successful, 0 if not
 *
 * REQUIRES lhs and rhs are not null
 *
 * NOTE scale is not handled
 * NOTE carry is handled automatically
 *
 * MODIFIES capacity, size and arr of lhs
 */
int STP_Number_add(STP_Number* lhs, STP_Number* _rhs);

int STP_Number_sub(STP_Number* lhs, STP_Number* rhs);
int STP_Number_mul(STP_Number* lhs, STP_Number* rhs);

int STP_Number_div(STP_Number* lhs, const STP_Number* rhs, uint64_t decimal_places);
int STP_Number_mod(STP_Number* lhs, const STP_Number* rhs, STP_Number* quotient);

int STP_Number_lshift(STP_Number* num, uint64_t bits);
int STP_Number_rshift(STP_Number* num, uint64_t bits);
