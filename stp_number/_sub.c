#include "_utils.h"
#include "stp_number.h"

int STP_Number_sub(STP_Number* lhs, STP_Number* rhs)
{
    /*
     * 1. Shortcuts
     * LHS = 0 => -RHS
     * RHS = 0 => nop
     *
     * LHS == RHS => zero out LHS
     *
     * 2. Ensure same sign
     * LHS = l, RHS = -r => l - (-r) = l + r
     * LHS = -l, RHS = r => -l - r = -(l + r)
     * add, sign depend on LHS
     *
     * 3. Output sign
     * |LHS| > |RHS| => out > 0
     * |LHS| < |RHS| => out < 0
     */
    if (lhs == NULL || rhs == NULL)
        return 0;

    if (STP_Number_is_zero(rhs))
        return 1;
    if (STP_Number_is_zero(lhs))
    {
        if (!STP_Number_copy(rhs, lhs))
            return 0;
        lhs->sign = -lhs->sign;
        return 1;
    }

    int cmp_result = STP_Number_cmp(lhs, rhs);
    if (cmp_result == 0)
    {
        STP_Number_clear(lhs);
        return 1;
    }

    if (lhs->sign != rhs->sign)
    {
        int original_rhs_sign = rhs->sign;
        rhs->sign = lhs->sign;

        int success = STP_Number_add(lhs, rhs);

        rhs->sign = original_rhs_sign;
        return success;
    }

    /*
     * If positive: cmp_result == -1 means |lhs| < |rhs|
     * If negative: cmp_result == 1  means |lhs| < |rhs|
     */
    int swap_required = (lhs->sign > 0) ? (cmp_result == -1) : (cmp_result == 1);

    if (swap_required)
    {
        STP_Number rhs_copy;
        if (!STP_Number_copy(rhs, &rhs_copy))
            return 0;

        if (!STP_Number_sub(&rhs_copy, lhs))
        {
            STP_Number_destroy(&rhs_copy);
            return 0;
        }

        rhs_copy.sign = (cmp_result > 0) ? 1 : -1;
        free(lhs->arr);
        *lhs = rhs_copy;
    }
    else
    {
        /* Core Loop */
        uint64_t borrow = 0;
        uint64_t max_result_size = lhs->size;

        for (uint64_t i = 0; i < max_result_size; i++)
        {
            uint64_t block_l = lhs->arr[i];
            uint64_t block_r = (i < rhs->size) ? rhs->arr[i] : 0;

            uint64_t res = block_l - block_r - borrow;

            if (borrow > 0)
                borrow = (block_l <= block_r || block_l - borrow < block_r) ? 1 : 0;
            else
                borrow = (block_l < block_r) ? 1 : 0;

            lhs->arr[i] = res;
        }

        lhs->sign = (cmp_result > 0) ? 1 : -1;
        _STP_Number_trim(lhs);
    }

    return 1;
}
