#include "stp_number.h"

#include "_utils.h"
#include "helpers.h"

#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int STP_Number_init_capacity(STP_Number* num, uint64_t capacity)
{
    if (num == NULL)
        return 0;

    num->sign = 1;
    num->scale = 0;
    num->arr = (uint64_t*)calloc(capacity, sizeof(uint64_t));
    if (num->arr == NULL)
    {
        fprintf(stderr, "%s: calloc failed", STP_CURRENT_FUNCTION);
        STP_ERRMSG(STP_CURRENT_FUNCTION, errno);
        num->size = 0;
        num->capacity = 0;
        return 0;
    }

    num->capacity = capacity;
    num->size = 1;

    return 1;
}

int STP_Number_init(STP_Number* num) { return STP_Number_init_capacity(num, _STP_NUMBER_DEFAULT_CAPACITY); }

int STP_Number_conv(STP_Number* num, const char* from)
{
    /*
     * NOTE
     * This is not the C++ Steppable. Number format support is very limited.
     * Ensure string is NULL-terminated or expect out-of-bounds memory access.
     *
     * Supported formats
     * [formatting spaces...] [sign] [digits...][.][digits...]
     * [formatting spaces...] [.][digits...]
     *
     * Define DECIMAL_SEP in stp_number.h to support other decimal separators.
     */

    if (num == NULL || from == NULL)
        return 0;

    int sign = 1;
    int encountered_decimal = 0;
    int encountered_digit = 0;

    int64_t start_idx = -1;
    int64_t end_idx = -1;
    int64_t decimal_idx = -1;

    uint64_t str_len = strlen(from);

    /* read metadata */
    for (uint64_t i = 0; i < str_len; i++)
    {
        char c = from[i];

        if (c == ' ')
        {
            if (sign != 1 || encountered_digit || encountered_decimal)
            {
                fprintf(stderr, "%s: ill-formatted string\n", STP_CURRENT_FUNCTION);
                fprintf(stderr, "spaces after sign/digits");
                return 0;
            }
            continue;
        }

        if (c == '+' || c == '-')
        {
            if (sign != 1 || encountered_decimal || encountered_digit)
            {
                fprintf(stderr, "%s: ill-formatted string\n", STP_CURRENT_FUNCTION);
                fprintf(stderr, "misplaced/duplicate sign");
                return 0;
            }
            sign = (c == '-') ? -1 : 1;
            continue;
        }

        if (c == DECIMAL_SEP)
        {
            if (encountered_decimal)
            {
                fprintf(stderr, "%s: ill-formatted string\n", STP_CURRENT_FUNCTION);
                fprintf(stderr, "duplicate decimal");
                return 0;
            }
            encountered_decimal = 1;
            decimal_idx = (int64_t)i;
            continue;
        }

        if (c < '0' || c > '9')
        {
            fprintf(stderr, "%s: ill-formatted string\n", STP_CURRENT_FUNCTION);
            fprintf(stderr, "unknown character '%c'", c);
            return 0;
        }

        if (!encountered_digit)
        {
            start_idx = (int64_t)i;
            encountered_digit = 1;
        }
        end_idx = (int64_t)i;
    }

    if (!encountered_digit)
    {
        fprintf(stderr, "%s: ill-formatted string\n", STP_CURRENT_FUNCTION);
        fprintf(stderr, "no digits found");
        return 0;
    }

    uint64_t total_digits = (uint64_t)(end_idx - start_idx + 1);
    /* -1 decimal place */
    if (encountered_decimal && decimal_idx >= start_idx && decimal_idx <= end_idx)
        total_digits--;

    uint64_t num_blocks = (total_digits + 18) / 19;
    if (!STP_Number_init_capacity(num, num_blocks))
        return 0;

    int64_t scale = 0;
    if (encountered_decimal && end_idx > decimal_idx)
        scale = -(end_idx - decimal_idx);

    uint64_t limb_idx = 0;
    uint64_t current_block = 0;
    uint64_t place_multiplier = 1;
    uint64_t digits_in_block = 0;

    for (int64_t j = end_idx + 1; j >= start_idx + 1; j--)
    {
        int64_t i = j - 1;
        char c = from[i];

        if (c == DECIMAL_SEP)
            continue;

        uint64_t digit = (uint64_t)(c - '0');
        current_block += digit * place_multiplier;
        place_multiplier *= 10;
        digits_in_block++;

        /* parse every 19 digits */
        if (digits_in_block == 19)
        {
            num->arr[limb_idx++] = current_block;
            current_block = 0;
            place_multiplier = 1;
            digits_in_block = 0;
        }
    }

    if (digits_in_block > 0)
        num->arr[limb_idx++] = current_block;

    num->size = limb_idx;
    num->sign = sign;
    num->scale = scale;

    return _STP_Number_trim(num);
}

int STP_Number_copy(const STP_Number* num, STP_Number* rhs)
{
    if (num == NULL || rhs == NULL)
        return 0;
    if (num->arr == NULL || rhs->arr == NULL)
        return 0;
    if (num == rhs)
        return 0;

    if (!_STP_Number_ensure_capacity(rhs, num->capacity))
        return 0;

    rhs->scale = num->scale;
    rhs->sign = num->sign;
    rhs->size = num->size;

    memcpy(rhs->arr, num->arr, num->capacity * sizeof(uint64_t));

    return 1;
}

int STP_Number_destroy(STP_Number* num)
{
    if (num == NULL)
        return 0;

    free(num->arr);
    num->arr = NULL;
    num->size = 0;
    num->capacity = 0;
    num->scale = 0;
    num->sign = 1;

    return 1;
}

int STP_Number_clear(STP_Number* num)
{
    if (num == NULL || num->arr == NULL)
        return 0;

    memset(num->arr, 0, num->size * sizeof(uint64_t));
    num->size = 1;
    num->scale = 0;
    num->sign = 1;
    return 1;
}

int STP_Number_set(STP_Number* num, uint64_t block)
{
    if (num == NULL || num->arr == NULL)
        return 0;
    if (num->size == 0)
    {
        _STP_Number_ensure_capacity(num, 1);
        num->size = 1;
    }

    num->arr[0] = block;
    num->size = 1;
    num->sign = 1;
    num->scale = 0;
    return 1;
}
