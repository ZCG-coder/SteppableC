#include "_utils.h"
#include "helpers.h"
#include "stp_number.h"
#include "stp_string.h"

#include <stdint.h>
#include <stdio.h>

#define DIGITS_BUF_DEFAULT_SIZE 256

int STP_Number_print(const STP_Number* num, STP_String* out)
{
    if (num == NULL || num->arr == NULL)
    {
        *out = STP_String_lit("(invalid)");
        return 0;
    }

    if (STP_Number_is_zero(num))
    {
        *out = STP_String_lit(" 0.");
        return 1;
    }

    STP_Number temp;
    STP_Number_copy(num, &temp);

    char* digits = (char*)calloc(DIGITS_BUF_DEFAULT_SIZE, sizeof(char));
    uint64_t digits_idx = 0;
    uint64_t digits_size = DIGITS_BUF_DEFAULT_SIZE;

    while (!STP_Number_is_zero(&temp))
    {
        uint8_t digit = _STP_Number_mod10(&temp);
        digits[digits_idx++] = '0' + (char)digit;

        if (digits_idx == digits_size)
        {
            /* Expand buffer */
            char* digits_new = (char*)realloc(digits, (digits_size << 1) * sizeof(char));
            if (digits_new == NULL)
            {
                free(digits);
                STP_Number_destroy(&temp);

                fprintf(stderr, "%s: realloc failed.", STP_CURRENT_FUNCTION);

                return 0;
            }
            digits_size <<= 1;
            digits = digits_new;
        }
    }

    STP_Number_destroy(&temp);

    int64_t scale = num->scale;
    uint64_t out_size = 0;

    /*
     * Exemplars of output
     * - integer
     *   format: " 110000."
     * - 0 < |decimal| < 1
     *   format: "-0.11"
     * - |decimal| > 1
     *   format: " 17.45"
     *
     * Memory allocation
     * [sign] + [digits] + [trailing zeros] + [decimal dot] + [NULL]
     * " "      "11"       "0000"             "."             "\0"
     */
    if (scale >= 0)
    {
        out_size = 1 + digits_idx + scale + 1 + 1;
    }
    else
    {
        int64_t total_fractional_places = -scale;
        if (digits_idx <= (uint64_t)total_fractional_places)
            out_size = 1 + 1 + 1 + (uint64_t)total_fractional_places + 1;
        else
            out_size = 1 + digits_idx + 1 + 1;
    }

    char* out_buf = malloc(out_size * sizeof(char));
    if (out_buf == NULL)
    {
        free(digits);
        fprintf(stderr, "%s: out_buf allocation failed.", STP_CURRENT_FUNCTION);
        return 0;
    }
    uint64_t out_idx = 0;

    if (num->sign < 0)
        out_buf[out_idx++] = '-';
    else
        out_buf[out_idx++] = ' ';

    if (scale >= 0)
    {
        for (int64_t i = (int64_t)digits_idx - 1; i >= 0; i--)
            out_buf[out_idx++] = digits[i];
        for (int64_t i = 0; i < scale; i++)
            out_buf[out_idx++] = '0';
        out_buf[out_idx++] = DECIMAL_SEP;
    }
    else
    {
        int64_t total_fractional_places = -scale;

        if (digits_idx <= (uint64_t)total_fractional_places)
        {
            out_buf[out_idx++] = '0';
            out_buf[out_idx++] = DECIMAL_SEP;

            uint64_t leading_zeros = (uint64_t)total_fractional_places - digits_idx;
            for (uint64_t i = 0; i < leading_zeros; i++)
                out_buf[out_idx++] = '0';
            for (int64_t i = (int64_t)digits_idx - 1; i >= 0; i--)
                out_buf[out_idx++] = digits[i];
        }
        else
        {
            uint64_t integer_digits = digits_idx - (uint64_t)total_fractional_places;

            uint64_t written = 0;
            for (int64_t i = (int64_t)digits_idx - 1; written < integer_digits; i--, written++)
                out_buf[out_idx++] = digits[i];

            out_buf[out_idx++] = DECIMAL_SEP;

            for (int64_t i = (int64_t)digits_idx - 1 - integer_digits; i >= 0; i--)
                out_buf[out_idx++] = digits[i];
        }
    }

    out_buf[out_idx] = '\0';
    *out = STP_String_lit(out_buf);

    free(digits);
    free(out_buf);

    return 1;
}
