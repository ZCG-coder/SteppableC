#include "_utils.h"
#include "stp_number.h"

/* precondition: |lhs| >= |rhs| */
int _STP_Number_sub_abs(STP_Number* lhs, const STP_Number* rhs)
{
    uint64_t i, borrow = 0;

    if (lhs == NULL || rhs == NULL)
        return 0;

    for (i = 0; i < lhs->size; ++i)
    {
        uint64_t a = lhs->arr[i];
        uint64_t b = (i < rhs->size) ? rhs->arr[i] : 0;

        uint64_t t = a - b;
        uint64_t b1 = (a < b) ? 1ULL : 0ULL;

        uint64_t u = t - borrow;
        uint64_t b2 = (t < borrow) ? 1ULL : 0ULL;

        lhs->arr[i] = u;
        borrow = (b1 | b2);
    }

    if (borrow != 0)
        return 0;

    return _STP_Number_trim(lhs);
}

int STP_Number_sub(STP_Number* lhs, STP_Number* rhs)
{
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

    /* Different signs: l - (-r) = l + r,  -l - r = -(l + r) */
    if (lhs->sign != rhs->sign)
    {
        int original_rhs_sign = rhs->sign;
        rhs->sign = lhs->sign;

        int ok = STP_Number_add(lhs, rhs);

        rhs->sign = original_rhs_sign;
        return ok;
    }

    /* Same-sign path: align scales, then subtract magnitudes */
    STP_Number rhs_aligned;
    if (!STP_Number_init(&rhs_aligned))
        return 0;

    if (!STP_Number_copy(rhs, &rhs_aligned))
        goto fail;

    if (!_STP_Number_align_scales(lhs, &rhs_aligned))
        goto fail;

    int lhs_sign = lhs->sign;
    int cmp_abs = STP_Number_cmp_abs(lhs, &rhs_aligned);

    if (cmp_abs == 0)
    {
        STP_Number_clear(lhs);
        STP_Number_destroy(&rhs_aligned);
        return 1;
    }

    if (cmp_abs > 0)
    {
        /* |lhs| > |rhs| => sign keeps lhs_sign */
        if (!_STP_Number_sub_abs(lhs, &rhs_aligned))
            goto fail;
        lhs->sign = lhs_sign;
    }
    else
    {
        /* |lhs| < |rhs| => result magnitude = |rhs|-|lhs|, sign flips */
        STP_Number lhs_copy;
        if (!STP_Number_init(&lhs_copy))
            goto fail;

        if (!STP_Number_copy(lhs, &lhs_copy))
        {
            STP_Number_destroy(&lhs_copy);
            goto fail;
        }

        if (!STP_Number_copy(&rhs_aligned, lhs))
        {
            STP_Number_destroy(&lhs_copy);
            goto fail;
        }

        if (!_STP_Number_sub_abs(lhs, &lhs_copy))
        {
            STP_Number_destroy(&lhs_copy);
            goto fail;
        }

        lhs->sign = -lhs_sign;
        STP_Number_destroy(&lhs_copy);
    }

    STP_Number_destroy(&rhs_aligned);
    return 1;

fail:
    STP_Number_destroy(&rhs_aligned);
    return 0;
}
