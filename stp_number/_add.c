#include "_utils.h"
#include "stp_number.h"

#include <stdint.h>
#include <stdlib.h>

uint64_t _STP_add64_carry(uint64_t* acc, uint64_t value)
{
    uint64_t diff = _BASE_10_19 - *acc;
    if (value >= diff)
    {
        *acc = value - diff;
        return 1ULL;
    }

    *acc += value;
    return 0ULL;
}

int _STP_Number_add(STP_Number* num, uint64_t val)
{
    if (num == NULL || num->arr == NULL || num->size == 0)
        return 0;

    uint64_t carry = val / _BASE_10_19;
    uint64_t remainder = val % _BASE_10_19;

    carry += _STP_add64_carry(&num->arr[0], remainder);

    for (uint64_t i = 1; i < num->size && carry > 0; i++)
        carry = _STP_add64_carry(&num->arr[i], carry);

    if (carry > 0)
    {
        if (!_STP_Number_ensure_capacity(num, num->size + 1))
            return 0;

        num->arr[num->size++] = carry;
    }

    return 1;
}

int _STP_Number_add_abs(STP_Number* lhs, const STP_Number* rhs)
{
    if (lhs == NULL || rhs == NULL || lhs->arr == NULL || rhs->arr == NULL)
        return 0;

    uint64_t max_size = (lhs->size > rhs->size) ? lhs->size : rhs->size;
    if (!_STP_Number_ensure_capacity(lhs, max_size + 1))
        return 0;

    uint64_t carry = 0;

    for (uint64_t i = 0; i < max_size; ++i)
    {
        uint64_t a = (i < lhs->size) ? lhs->arr[i] : 0;
        uint64_t b = (i < rhs->size) ? rhs->arr[i] : 0;

        /* b + carry <= 10^19 */
        uint64_t limit = _BASE_10_19 - b - carry;
        if (a >= limit)
        {
            lhs->arr[i] = a - limit;
            carry = 1;
        }
        else
        {
            lhs->arr[i] = a + b + carry;
            carry = 0;
        }
    }

    if (carry > 0)
    {
        lhs->arr[max_size] = carry;
        lhs->size = max_size + 1;
    }
    else
    {
        lhs->size = max_size;
    }

    return _STP_Number_trim(lhs);
}

int STP_Number_add(STP_Number* lhs, STP_Number* _rhs)
{
    if (lhs == NULL || _rhs == NULL)
        return 0;
    if (lhs->arr == NULL || _rhs->arr == NULL)
        return 0;

    if (lhs->sign != _rhs->sign)
    {
        /*
         * Temporarily match RHS sign to LHS sign.
         */
        int original_rhs_sign = _rhs->sign;
        _rhs->sign = lhs->sign;

        int success = STP_Number_sub(lhs, _rhs);

        _rhs->sign = original_rhs_sign;
        return success;
    }

    STP_Number rhs;
    if (!STP_Number_init(&rhs))
        return 0;

    if (!STP_Number_copy(_rhs, &rhs))
        goto fail;

    if (!_STP_Number_align_scales(lhs, &rhs))
        goto fail;

    /* Same-sign addition of magnitudes */
    if (!_STP_Number_add_abs(lhs, &rhs))
        goto fail;

    lhs->sign = (_rhs->sign >= 0) ? 1 : -1;

    STP_Number_destroy(&rhs);
    return 1;

fail:
    STP_Number_destroy(&rhs);
    return 0;
}
