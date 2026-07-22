#include "_utils.h"
#include "stp_number.h"

#include <stdint.h>
#include <stdlib.h>

uint64_t _STP_add64_carry(uint64_t* acc, uint64_t value)
{
    uint64_t old_value = *acc;
    *acc = old_value + value;
    return (*acc < old_value) ? 1ULL : 0ULL;
}

int _STP_Number_add(STP_Number* num, uint64_t val)
{
    if (num == NULL || num->arr == NULL || num->size == 0)
        return 0;

    uint64_t carry = val;

    for (uint64_t i = 0; i < num->size && carry > 0; i++)
    {
        uint64_t old_val = num->arr[i];
        num->arr[i] += carry;

        /* When overflow, carry to next block */
        if (num->arr[i] < old_val)
            carry = 1;
        else
            carry = 0;
    }

    if (carry > 0)
    {
        if (!_STP_Number_ensure_capacity(num, num->size + 1))
            return 0;

        num->arr[num->size] = carry;
        num->size++;
    }

    return 1;
}

int _STP_Number_add_abs(STP_Number* lhs, const STP_Number* rhs)
{
    uint64_t i, max_size, carry = 0;

    if (lhs == NULL || rhs == NULL)
        return 0;
    if (lhs->arr == NULL || rhs->arr == NULL)
        return 0;

    max_size = (lhs->size > rhs->size) ? lhs->size : rhs->size;
    if (!_STP_Number_ensure_capacity(lhs, max_size + 1))
        return 0;

    for (i = 0; i < max_size; ++i)
    {
        uint64_t a = (i < lhs->size) ? lhs->arr[i] : 0;
        uint64_t b = (i < rhs->size) ? rhs->arr[i] : 0;

        uint64_t s1 = a + b;
        uint64_t c1 = (s1 < a || s1 < b) ? 1ULL : 0ULL;

        uint64_t s2 = s1 + carry;
        uint64_t c2 = (s2 < s1 || s2 < carry) ? 1ULL : 0ULL;

        lhs->arr[i] = s2;
        carry = c1 | c2;
    }

    lhs->arr[max_size] = carry;
    lhs->size = max_size + (carry ? 1 : 0);
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
