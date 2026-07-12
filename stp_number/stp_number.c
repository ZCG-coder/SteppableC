#include "stp_number.h"

#include "_utils.h"
#include "helpers.h"
#include "stp_string.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int STP_Number_init(STP_Number* num)
{
    if (num == NULL)
        return 0;

    num->sign = 1;
    num->scale = 0;
    num->arr = (uint64_t*)calloc(_STP_NUMBER_DEFAULT_CAPACITY, sizeof(uint64_t));
    if (num->arr == NULL)
    {
        num->size = 0;
        num->capacity = 0;
        return 0;
    }

    num->capacity = _STP_NUMBER_DEFAULT_CAPACITY;
    num->size = 1;

    return 1;
}

int STP_Number_conv(STP_Number* num, const char* from)
{
    if (!STP_Number_init(num))
        return 0;

    /*
     * NOTE
     * This is not the C++ Steppable. Integer format support is very limited.
     * Ensure string is NULL-terminated or expect out-of-bounds memory access.
     *
     * Supported formats
     * [formatting spaces...] [sign] [digits...][.][digits...]
     * [formatting spaces...] [.][digits...]
     *
     * Define DECIMAL_SEP in stp_number.h to support other decimal separators.
     */
    int sign = 0;
    int encountered_decimal = 0;
    int encountered_digit = 0;

    for (uint64_t i = 0; from[i] != '\0'; i++)
    {
        char c = from[i];
        if (c == ' ')
        {
            if (sign != 0 || encountered_digit || encountered_decimal)
            {
                fprintf(stderr, "%s: ill-formatted string.\n", STP_CURRENT_FUNCTION);
                fprintf(stderr, "%s: spaces after sign.\n", STP_CURRENT_FUNCTION);
                return 0;
            }
            continue;
        }

        if (c == DECIMAL_SEP)
        {
            if (encountered_decimal)
            {
                fprintf(stderr, "%s: ill-formatted string.\n", STP_CURRENT_FUNCTION);
                fprintf(stderr, "%s: duplicated decimal separators.\n", STP_CURRENT_FUNCTION);
                return 0;
            }
            encountered_decimal = 1;
            continue;
        }

        if ((c == '+' || c == '-') && (sign != 0 || encountered_decimal || encountered_digit))
        {
            fprintf(stderr, "%s: ill-formatted string.\n", STP_CURRENT_FUNCTION);
            fprintf(stderr, "%s: duplicated sign %c.\n", STP_CURRENT_FUNCTION, c);
            return 0;
        }

        if (c == '-')
        {
            sign = -1;
            continue;
        }
        if (c == '+')
        {
            sign = 1;
            continue;
        }

        if (!('0' <= c && c <= '9'))
        {
            fprintf(stderr, "%s: ill-formatted string.\n", STP_CURRENT_FUNCTION);
            fprintf(stderr, "%s: unknown character.\n", STP_CURRENT_FUNCTION);
            return 0;
        }

        uint8_t digit = c - '0';
        encountered_digit = 1;

        if (!_STP_Number_mul_exp(num, 1))
            return 0;

        if (!_STP_Number_add(num, digit))
            return 0;

        if (encountered_decimal)
            num->scale--;
    }

    if (sign == 0)
        sign = 1;
    num->sign = sign;
    return 1;
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

int main()
{
    STP_Number n1;
    (void)STP_Number_conv(&n1, "55340232221128654897.7484732");

    STP_Number n2;
    (void)STP_Number_conv(&n2, "673292392.4319814");

    STP_String str;
    STP_String_init(&str);

    STP_Number_print(&n1, &str);
    printf("n1 = %s\n", str.str);
    STP_Number_print(&n2, &str);
    printf("n2 = %s\n", str.str);

    STP_Number_add(&n1, &n2);
    STP_Number_print(&n1, &str);
    printf("n1+n2 = %s\n", str.str);

    STP_Number_destroy(&n1);
    STP_Number_destroy(&n2);
    STP_String_destroy(&str);
    return 0;

fail:
    STP_Number_destroy(&n1);
    STP_Number_destroy(&n2);
    STP_String_destroy(&str);
    return 1;
}
