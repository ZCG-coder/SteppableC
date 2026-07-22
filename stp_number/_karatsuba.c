#include "_mul.h"
#include "_utils.h"
#include "stp_number.h"

#include <string.h>

int _STP_Number_add_shifted(STP_Number* out, const STP_Number* add, uint64_t shift)
{
    STP_Number shifted;

    if (out == NULL || add == NULL)
        return 0;
    if (out->arr == NULL || add->arr == NULL)
        return 0;

    if (add->size == 0)
        return 1;

    if (!STP_Number_init(&shifted))
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
    STP_Number sx, sy;
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

    if (!STP_Number_init(&x0))
        return 0;
    if (!STP_Number_init(&x1))
        goto fail_x0;
    if (!STP_Number_init(&y0))
        goto fail_x1;
    if (!STP_Number_init(&y1))
        goto fail_y0;
    if (!STP_Number_init(&z0))
        goto fail_y1;
    if (!STP_Number_init(&z1))
        goto fail_z0;
    if (!STP_Number_init(&z2))
        goto fail_z1;
    if (!STP_Number_init(&sx))
        goto fail_z2;
    if (!STP_Number_init(&sy))
        goto fail_sx;

    if (!_STP_Number_slice(&x0, lhs, 0, m))
        goto fail;
    if (!_STP_Number_slice(&x1, lhs, m, lhs->size - m))
        goto fail;
    if (!_STP_Number_slice(&y0, rhs, 0, m))
        goto fail;
    if (!_STP_Number_slice(&y1, rhs, m, rhs->size - m))
        goto fail;

    if (!_STP_Number_mul_abs_karatsuba(&z0, &x0, &y0))
        goto fail;
    if (!_STP_Number_mul_abs_karatsuba(&z2, &x1, &y1))
        goto fail;

    if (!STP_Number_copy(&x0, &sx))
        goto fail;
    if (!_STP_Number_add_abs(&sx, &x1))
        goto fail;

    if (!STP_Number_copy(&y0, &sy))
        goto fail;
    if (!_STP_Number_add_abs(&sy, &y1))
        goto fail;

    if (!_STP_Number_mul_abs_karatsuba(&z1, &sx, &sy))
        goto fail;

    if (!_STP_Number_sub_abs(&z1, &z0))
        goto fail;
    if (!_STP_Number_sub_abs(&z1, &z2))
        goto fail;

    if (!_STP_Number_ensure_capacity(out, lhs->size + rhs->size))
        goto fail;
    out->size = lhs->size + rhs->size;
    out->scale = 0;
    out->sign = 1;
    memset(out->arr, 0, out->size * sizeof(uint64_t));

    if (!_STP_Number_add_shifted(out, &z0, 0))
        goto fail;
    if (!_STP_Number_add_shifted(out, &z1, m))
        goto fail;
    if (!_STP_Number_add_shifted(out, &z2, 2 * m))
        goto fail;

    if (!_STP_Number_trim(out))
        goto fail;
    out->sign = 1;
    out->scale = 0;

    STP_Number_destroy(&sy);
    STP_Number_destroy(&sx);
    STP_Number_destroy(&z2);
    STP_Number_destroy(&z1);
    STP_Number_destroy(&z0);
    STP_Number_destroy(&y1);
    STP_Number_destroy(&y0);
    STP_Number_destroy(&x1);
    STP_Number_destroy(&x0);
    return 1;

fail:
    STP_Number_destroy(&sy);
fail_sx:
    STP_Number_destroy(&sx);
fail_z2:
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
