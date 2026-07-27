#include "_utils.h"
#include "helpers.h"
#include "stp_number.h"

#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int STP_Number_ln(STP_Number* n, int64_t wp)
{
    if (n == NULL || n->arr == NULL)
        return 0;

    if (n->sign <= 0 || STP_Number_is_zero(n))
    {
        fprintf(stderr, "%s: log of non-positive number\n", STP_CURRENT_FUNCTION);
        return 0;
    }

    _STP_Number_trim(n);

    if (n->arr[0] == 1 && n->size == 1 && n->scale == 0)
    {
        STP_Number_clear(n);
        return 1;
    }

    STP_Number guess, neg_guess, one;
    STP_Number_init(&neg_guess);
    STP_Number_init(&one);

    if (!STP_Number_set(&one, 1))
        goto fail;

    long double top_block = (long double)n->arr[n->size - 1];
    if (n->size > 1)
        top_block += (long double)n->arr[n->size - 2] / (long double)_BASE_10_19;

    int64_t exp10 = (int64_t)19 * (n->size - 1) + n->scale;
    const long double LN10 = 2.30258509299404568401799145468436L;
    long double guess_val = log(top_block) + (long double)exp10 * LN10;

    char buffer[64];
    snprintf(buffer, sizeof(buffer), "%.15Lf", guess_val);
    if (!STP_Number_conv(&guess, buffer))
        goto fail;

    int64_t current_wp = 15;
    uint64_t n_digits = _STP_Number_count_digits(n);

    /* y_(n + 1) = y_n + n * exp(-y_n) - 1 */
    while (1)
    {
        int is_final = 0;
        if (current_wp >= wp)
        {
            current_wp = wp;
            is_final = 1;
        }

        int64_t iter_wp = current_wp + 8;

        /* neg_guess = -guess */
        if (!STP_Number_copy(&guess, &neg_guess))
            goto fail;
        neg_guess.sign = -1;

        if (!STP_Number_exp(&neg_guess, iter_wp + n_digits))
            goto fail;

        /* neg_guess = n * exp(-guess) */
        if (!STP_Number_mul(&neg_guess, n))
            goto fail;
        STP_Number_round(&neg_guess, iter_wp + n_digits);

        /* guess = guess + (n * exp(-guess)) - 1 */
        if (!STP_Number_add(&guess, &neg_guess) || !STP_Number_sub(&guess, &one))
            goto fail;

        if (is_final)
            break;

        current_wp *= 2;
    }

    STP_Number_round(&guess, wp);
    free(n->arr);
    *n = guess;

    STP_Number_destroy(&neg_guess);
    STP_Number_destroy(&one);
    return 1;

fail:
    STP_Number_destroy(&guess);
    STP_Number_destroy(&neg_guess);
    STP_Number_destroy(&one);
    return 0;
}
