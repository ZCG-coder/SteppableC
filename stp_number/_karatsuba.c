#include "_mul.h"
#include "_shift.h"
#include "_utils.h"
#include "stp_number.h"

#include <alloc.h>
#include <stdint.h>
#include <string.h>

int _add_shifted(STP_Number* out, const STP_Number* add, uint64_t shift)
{
    STP_Number shifted;

    if (out == NULL || add == NULL)
        return 0;
    if (out->arr == NULL || add->arr == NULL)
        return 0;

    if (add->size == 0)
        return 1;

    if (!STP_Number_init_capacity(&shifted, add->size + shift))
        return 0;

    if (!STP_Number_copy(add, &shifted))
        goto fail;

    shifted.sign = 1;
    shifted.scale = 0;

    if (!_STP_Number_lshift_blocks(&shifted, shift))
        goto fail;

    if (!_STP_Number_add_abs(out, &shifted))
        goto fail;

    out->sign = 1;
    out->scale = 0;

    STP_Number_destroy(&shifted);
    return 1;

fail:
    STP_Number_destroy(&shifted);
    return 0;
}

/**
 * Multiplies lhs and rhs using Karatsuba algorithm, returning result in out
 *
 * ARG lhs multiplicand
 * ARG rhs multiplier
 * ARG out product output
 * RETURNS 1 if successful, 0 if fail
 *
 * REQUIRES lhs and rhs are not NULL
 * REQUIRES out is initialized
 */
int _STP_Number_mul_abs_karatsuba(STP_Number* out, const STP_Number* lhs, const STP_Number* rhs)
{
    STP_Number x0, x1, y0, y1;
    STP_Number z0, z1, z2;
    uint64_t n, m;

    if (out == NULL || lhs == NULL || rhs == NULL)
        return 0;
    if (out->arr == NULL || lhs->arr == NULL || rhs->arr == NULL)
        return 0;

    if (lhs->size == 0 || rhs->size == 0)
    {
        out->size = 0;
        out->scale = 0;
        out->sign = 1;
        return 1;
    }

    n = (lhs->size > rhs->size) ? lhs->size : rhs->size;

    if (n <= 1)
        return _STP_Number_mul_abs_schoolbook(out, lhs, rhs);
    if (lhs->size < STP_MUL_KARATSUBA_THRESHOLD || rhs->size < STP_MUL_KARATSUBA_THRESHOLD)
        return _STP_Number_mul_abs_schoolbook(out, lhs, rhs);

    m = n / 2;
    if (m == 0)
        return _STP_Number_mul_abs_schoolbook(out, lhs, rhs);

    uint64_t x1_size = (lhs->size > m) ? lhs->size - m : 0;
    uint64_t y1_size = (rhs->size > m) ? rhs->size - m : 0;

    if (!STP_Number_init_capacity(&x0, m))
        return 0;
    if (!STP_Number_init_capacity(&x1, x1_size))
        goto fail_x0;
    if (!STP_Number_init_capacity(&y0, m))
        goto fail_x1;
    if (!STP_Number_init_capacity(&y1, y1_size))
        goto fail_y0;
    if (!STP_Number_init_capacity(&z0, m + x1_size))
        goto fail_y1;
    if (!STP_Number_init(&z1))
        goto fail_z0;
    if (!STP_Number_init_capacity(&z2, x1_size + y1_size))
        goto fail_z1;

    if (!_STP_Number_slice(&x0, lhs, 0, m))
        goto fail;
    if (!_STP_Number_slice(&x1, lhs, m, x1_size))
        goto fail;
    if (!_STP_Number_slice(&y0, rhs, 0, m))
        goto fail;
    if (!_STP_Number_slice(&y1, rhs, m, y1_size))
        goto fail;

    if (!_STP_Number_mul_abs_karatsuba(&z0, &x0, &y0))
        goto fail;
    if (!_STP_Number_mul_abs_karatsuba(&z2, &x1, &y1))
        goto fail;

    if (!_STP_Number_add_abs(&x0, &x1))
        goto fail;

    if (!_STP_Number_add_abs(&y0, &y1))
        goto fail;

    if (!_STP_Number_mul_abs_karatsuba(&z1, &x0, &y0))
        goto fail;

    if (!_STP_Number_sub_abs(&z1, &z0))
        goto fail;
    if (!_STP_Number_sub_abs(&z1, &z2))
        goto fail;

    /*
    DO NOT RESIZE OUT
    already allocated enough memory to perform operations
    */
    out->size = lhs->size + rhs->size;
    out->scale = 0;
    out->sign = 1;

    if (!_add_shifted(out, &z0, 0))
        goto fail;
    if (!_add_shifted(out, &z1, m))
        goto fail;
    if (!_add_shifted(out, &z2, 2 * m))
        goto fail;

    if (!_STP_Number_trim(out))
        goto fail;
    out->sign = 1;
    out->scale = 0;

    STP_Number_destroy(&z2);
    STP_Number_destroy(&z1);
    STP_Number_destroy(&z0);
    STP_Number_destroy(&y1);
    STP_Number_destroy(&y0);
    STP_Number_destroy(&x1);
    STP_Number_destroy(&x0);
    return 1;

fail:
    STP_Number_destroy(&z2);
fail_z1:
    STP_Number_destroy(&z1);
fail_z0:
    STP_Number_destroy(&z0);
fail_y1:
    STP_Number_destroy(&y1);
fail_y0:
    STP_Number_destroy(&y0);
fail_x1:
    STP_Number_destroy(&x1);
fail_x0:
    STP_Number_destroy(&x0);
    return 0;
}
