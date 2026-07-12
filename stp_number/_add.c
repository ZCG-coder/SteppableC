#include "_utils.h"
#include "helpers.h"
#include "stp_number.h"

#include <stdint.h>

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

int STP_Number_add(STP_Number* lhs, const STP_Number* _rhs)
{
    if (lhs == NULL || _rhs == NULL)
        return 0;

    /* Ensure signs match */
    if (lhs->sign > _rhs->sign)
    {
    }

    STP_Number rhs;
    STP_Number_init(&rhs);
    if (!STP_Number_copy(_rhs, &rhs))
        return 0;

    if (!_STP_Number_align_scales(lhs, &rhs))
        goto fail;

    uint64_t carry = 0;
    uint64_t max_result_size = (lhs->size > rhs.size) ? lhs->size + 1 : rhs.size + 1;
    if (!_STP_Number_ensure_capacity(lhs, max_result_size))
        goto fail;

    for (uint64_t i = 0; i < max_result_size; i++)
    {
        uint64_t block_l = lhs->arr[i];
        uint64_t block_r = (i < rhs.size) ? rhs.arr[i] : 0;

        uint64_t res = block_l + block_r + carry;
        if (carry > 0)
            carry = (res <= block_l || res <= block_r) ? 1 : 0;
        else
            carry = (res < block_l) ? 1 : 0;

        lhs->arr[i] = res;
    }

    lhs->size = max_result_size;
    _STP_Number_trim(lhs);

    STP_Number_destroy(&rhs);
    return 1;

fail:
    fprintf(stderr, "%s: add failed\n", STP_CURRENT_FUNCTION);
    STP_Number_destroy(&rhs);
    return 0;
}
