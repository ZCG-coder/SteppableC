#include "_utils.h"
#include "helpers.h"
#include "stp_number.h"

#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static const long double ln2 = 0.6931471805599453;

int STP_Number_ln(STP_Number* n, int64_t wp)
{
    if (n == NULL || n->arr == NULL)
        return 0;
    if (STP_Number_is_zero(n))
    {
        fprintf(stderr, "%s: log of zero", STP_CURRENT_FUNCTION);
        return 0;
    }

    if (n->arr[0] == 1 && n->size == 1 && n->scale == 0)
    {
        STP_Number_clear(n);
        return 1;
    }

    long double top_block = n->arr[n->size - 1];
    long double _guess = logl(top_block) + 64 * (n->size - 1) * ln2;
    char buffer[64];
    snprintf(buffer, sizeof(buffer), "%.15Lf", _guess);

    STP_Number guess;
    STP_Number_conv(&guess, buffer);
    int64_t current_wp = 15;
    int done = 0;

    STP_Number one;
    STP_Number_conv(&one, "1.0");

    STP_Number x;
    STP_Number_init(&x);

    STP_Number e_yn;
    STP_Number_init(&e_yn);

    while (!done)
    {
        if (current_wp >= wp)
        {
            current_wp = wp;
            done = 1;
        }

        STP_Number_copy(&guess, &e_yn);
        STP_Number_copy(n, &x);
        _STP_PRINT_NUM(guess, "guess");
        _STP_PRINT_NUM(e_yn, "e_yn");

        e_yn.sign = -1;
        STP_Number_exp(&e_yn, current_wp);
        STP_Number_mul(&x, &e_yn);

        STP_Number_add(&guess, &x);
        STP_Number_sub(&guess, &one);
        current_wp *= 2;
    }

    STP_Number_round(&guess, current_wp);
    free(n->arr);
    *n = guess;

    STP_Number_destroy(&x);
    STP_Number_destroy(&e_yn);
    STP_Number_destroy(&one);
    return 1;
}
