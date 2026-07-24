#include "_utils.h"
#include "helpers.h"
#include "stp_number.h"
#include "stp_string.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define DIGITS_BUF_DEFAULT_SIZE 256

char* _to_string(const STP_Number* num, uint64_t* len)
{
    STP_Number copy;
    STP_Number_init(&copy);
    if (!STP_Number_copy(num, &copy))
    {
        fprintf(stderr, "%s: cannot copy num", STP_CURRENT_FUNCTION);
        return NULL;
    }

    /* 2^64 = 18446744073709551616, 20 digits/block */
    uint64_t max_chars = (copy.size * 20) + 1;
    char* buffer = malloc(max_chars);
    if (!buffer)
    {
        fprintf(stderr, "%s: cannot create buffer", STP_CURRENT_FUNCTION);
        STP_Number_destroy(&copy);
        return NULL;
    }

    char* ptr = buffer + max_chars - 1;
    *ptr = '\0';

    const uint32_t chunk_base = 1E9L;

    while (1)
    {
        uint64_t remainder = _STP_Number_mod(&copy, chunk_base);

        if (copy.size == 0)
        {
            char temp[12];
            int tmp_len = snprintf(temp, sizeof(temp), "%llu", (uint64_t)remainder);
            ptr -= tmp_len;
            memcpy(ptr, temp, tmp_len);

            break;
        }
        else
        {
            char temp[12];
            snprintf(temp, sizeof(temp), "%09llu", (uint64_t)remainder);
            ptr -= 9;
            memcpy(ptr, temp, 9);
        }
    }

    /* shift bytes to front */
    size_t final_length = (buffer + max_chars) - ptr;
    memmove(buffer, ptr, final_length);

    if (len)
        *len = final_length - 1;

    STP_Number_destroy(&copy);
    return buffer;
}

int STP_Number_print(const STP_Number* num, STP_String* out)
{
    if (num == NULL || num->arr == NULL)
    {
        STP_String_assign_buf(out, "(invalid)");
        return 0;
    }

    if (STP_Number_is_zero(num))
        return STP_String_assign_buf(out, " 0.");

    uint64_t out_size = 0;
    char* digits = digits = _to_string(num, &out_size);
    if (digits == NULL)
        return 0;

    uint64_t digits_count = out_size;
    int64_t scale = num->scale;

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
        out_size += 1 + scale + 1 + 1;
    }
    else
    {
        int64_t dec_places = -scale;
        if (out_size <= (uint64_t)dec_places)
            out_size = 1 + 1 + 1 + (uint64_t)dec_places + 1;
        else
            out_size = 1 + out_size + 1 + 1;
    }

    char* out_buf = malloc(out_size * sizeof(char));
    if (out_buf == NULL)
    {
        free(digits);
        fprintf(stderr, "%s: out_buf allocation failed.\n", STP_CURRENT_FUNCTION);
        return 0;
    }
    uint64_t out_idx = 0;

    if (num->sign < 0)
        out_buf[out_idx++] = '-';
    else
        out_buf[out_idx++] = ' ';

    if (scale >= 0)
    {
        for (uint64_t i = 0; i < digits_count; i++)
            out_buf[out_idx++] = digits[i];
        for (int64_t i = 0; i < scale; i++)
            out_buf[out_idx++] = '0';
        out_buf[out_idx++] = DECIMAL_SEP;
    }
    else
    {
        int64_t total_fractional_places = -scale;

        if (digits_count <= (uint64_t)total_fractional_places)
        {
            out_buf[out_idx++] = '0';
            out_buf[out_idx++] = DECIMAL_SEP;

            uint64_t leading_zeros = (uint64_t)total_fractional_places - digits_count;
            for (uint64_t i = 0; i < leading_zeros; i++)
                out_buf[out_idx++] = '0';
            for (uint64_t i = 0; i < digits_count; i++)
                out_buf[out_idx++] = digits[i];
        }
        else
        {
            uint64_t integer_digits = digits_count - (uint64_t)total_fractional_places;
            for (uint64_t i = 0; i < integer_digits; i++)
                out_buf[out_idx++] = digits[i];

            out_buf[out_idx++] = DECIMAL_SEP;

            for (uint64_t i = integer_digits; i < digits_count; i++)
                out_buf[out_idx++] = digits[i];
        }
    }

    out_buf[out_idx] = '\0';
    STP_String_assign_buf(out, out_buf);

    free(digits);
    free(out_buf);

    return 1;
}
