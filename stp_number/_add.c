#include "_utils.h"
#include "stp_number.h"

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

int STP_Number_add(STP_Number* lhs, STP_Number* rhs)
{
    if (lhs == NULL || rhs == NULL)
        return 0;

    return 1;
}
