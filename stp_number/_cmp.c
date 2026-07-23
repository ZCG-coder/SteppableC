#include "_utils.h"
#include "helpers.h"
#include "stp_number.h"

#include <stdint.h>
#include <stdio.h>
#include <string.h>

int STP_Number_is_zero(const STP_Number* num)
{
    if (num == NULL || num->arr == NULL)
    {
        fprintf(stderr, "%s: Invalid number input\n", STP_CURRENT_FUNCTION);
        return 0;
    }
    if (num->size == 0)
        return 1;

    for (uint64_t idx = 0; idx < num->size; ++idx)
        if (num->arr[idx] != 0)
            return 0;

    return 1;
}

int _STP_Number_cmp_abs(const STP_Number* lhs, const STP_Number* rhs)
{
    uint64_t i;
    uint64_t lhs_size;
    uint64_t rhs_size;

    if (lhs == NULL || rhs == NULL)
        return 0; /* invalid input */

    lhs_size = lhs->size;
    while (lhs_size > 0 && lhs->arr[lhs_size - 1] == 0)
        --lhs_size;

    rhs_size = rhs->size;
    while (rhs_size > 0 && rhs->arr[rhs_size - 1] == 0)
        --rhs_size;

    if (lhs_size > rhs_size)
        return 1;
    if (lhs_size < rhs_size)
        return -1;

    for (i = lhs_size; i > 0; --i)
    {
        uint64_t a = lhs->arr[i - 1];
        uint64_t b = rhs->arr[i - 1];

        if (a > b)
            return 1;
        if (a < b)
            return -1;
    }

    return 0;
}

int STP_Number_cmp(STP_Number* lhs, STP_Number* rhs)
{
    if (lhs == NULL || rhs == NULL)
        return 42;

    /* Same address, same value */
    if (lhs == rhs)
        return 0;

    if (STP_Number_is_zero(lhs))
    {
        if (STP_Number_is_zero(rhs))
            return 0;
        if (rhs->sign > 0)
            return -1;
        if (rhs->sign < 0)
            return 1;
    }

    if (lhs->sign > rhs->sign)
        return 1;
    if (rhs->sign > lhs->sign)
        return -1;

    /* Signs equal, align scales */
    _STP_Number_trim(lhs);
    _STP_Number_trim(rhs);
    if (!_STP_Number_align_scales(lhs, rhs))
        return 42;

    int magnitude_cmp = 0;

    if (lhs->size > rhs->size)
    {
        magnitude_cmp = 1;
    }
    else if (rhs->size > lhs->size)
    {
        magnitude_cmp = -1;
    }
    else
    {
        for (uint64_t i = lhs->size; i > 0; i--)
        {
            uint64_t idx = i - 1;
            if (lhs->arr[idx] > rhs->arr[idx])
            {
                magnitude_cmp = 1;
                break;
            }
            if (rhs->arr[idx] > lhs->arr[idx])
            {
                magnitude_cmp = -1;
                break;
            }
        }
    }

    if (lhs->sign > 0)
        return magnitude_cmp;
    return -magnitude_cmp;
}
