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

int STP_Number_init(STP_Number* num);

int STP_Number_conv(STP_Number* num, const char* from);

int STP_Number_copy(const STP_Number* num, STP_Number* rhs);

int STP_Number_destroy(STP_Number* num);
int STP_Number_clear(STP_Number* num);

int STP_Number_print(const STP_Number* num, STP_String* out);

/*
 * Arithmetic operations
 */

int STP_Number_is_zero(const STP_Number* num);
int STP_Number_cmp(STP_Number* lhs, STP_Number* rhs);

int STP_Number_add(STP_Number* lhs, STP_Number* _rhs);
int STP_Number_sub(STP_Number* lhs, STP_Number* rhs);
int STP_Number_mul(STP_Number* lhs, STP_Number* rhs);
