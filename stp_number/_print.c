#include "helpers.h"
#include "stp_number.h"
#include "stp_string.h"

#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int _to_string(const STP_Number* num, STP_String* str)
{
    /* 19 digits/block + NULL */
    uint64_t max_chars = (num->size * 19) + 1;
    char* buffer = malloc(max_chars);
    if (!buffer)
    {
        fprintf(stderr, "%s: cannot create buffer", STP_CURRENT_FUNCTION);
        return 0;
    }
    size_t offset = 0;
    size_t initial_offset = 0;

    uint64_t msb = num->arr[num->size - 1];
    offset += sprintf(buffer + offset, "%" PRIu64, msb);
    initial_offset = offset;

    for (uint64_t i = num->size - 1; i >= 1; i--)
        offset += sprintf(buffer + offset, "%019" PRIu64, num->arr[i - 1]);

    str->str = buffer;
    str->length = max_chars - 19 + initial_offset - 1;
    str->capacity = max_chars;
    return 1;
}

int STP_Number_print(const STP_Number* num, STP_String* out)
{
    if (num == NULL || num->arr == NULL)
    {
        free(out->str);
        STP_String_assign_buf(out, "(invalid)");
        return 0;
    }

    if (STP_Number_is_zero(num))
    {
        free(out->str);
        return STP_String_assign_buf(out, " 0.");
    }

    STP_String digits;
    if (!_to_string(num, &digits))
        return 0;

    int64_t scale = num->scale;

    /*
     * Exemplars of output
     * - integer
     *   format: " 110000."
     * - 0 < |decimal| < 1
     *   format: "-0.11" (scale = -2, len = 2)
     *   format: "-0.01" (scale = -2, len = 1)
     * - |decimal| > 1
     *   format: " 17.456" (scale = -2, len = 5)
     */
    STP_String zero = STP_String_lit("0");
    STP_String dot = STP_String_lit((char[2]){ DECIMAL_SEP, '\0' });
    STP_String zerodot = STP_String_lit((char[3]){ '0', DECIMAL_SEP, '\0' });
    if (scale > 0)
    {
        for (int64_t i = 0; i < scale; ++i)
            STP_String_append(&digits, &zero);
        STP_String_append(&digits, &dot);
    }
    else if (scale < 0)
    {
        int64_t decimal_places = -scale;
        uint64_t len = digits.length;
        if (decimal_places >= len)
        {
            for (int64_t i = 0; i < (decimal_places - len); ++i)
                STP_String_prepend(&digits, &zero);
            STP_String_prepend(&digits, &zerodot);
        }
        else
        {
            /* place decimal dot */
            STP_String_insert(&digits, len - decimal_places, &dot);
        }
    }

    STP_String sign;
    if (num->sign < 0)
        sign = STP_String_lit("-");
    else
        sign = STP_String_lit(" ");
    STP_String_prepend(&digits, &sign);

    free(out->str);
    *out = digits;

    STP_String_destroy(&sign);
    STP_String_destroy(&zero);
    STP_String_destroy(&dot);
    STP_String_destroy(&zerodot);
    return 1;
}
