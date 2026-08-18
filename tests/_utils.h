#pragma once

#include "helpers.h"

#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>

#define DEFAULT_RANDOM_LEN 152

char* _generate_random_number(uint64_t len)
{
    if (len == 0)
        return NULL;

    char* out = malloc((len + 2) * sizeof(char)); /* +1 for NULL, +1 for sign */
    if (out == NULL)
    {
        STP_ERRMSG(STP_CURRENT_FUNCTION, errno);
        return NULL;
    }

    int placed_decimal = 0;

    for (uint64_t i = 1; i < len; ++i)
    {
        if (!placed_decimal && i > 1)
        {
            float place_decimal_now = (float)rand() / (float)RAND_MAX;
            if (place_decimal_now < (1.0F / len))
            {
                out[i] = '.';
                placed_decimal = 1;
                continue;
            }
        }

        int digit = rand() % 10;
        while (digit == 0 && i == 0)
            digit = rand() % 10;
        out[i] = (char)(digit + '0');
    }

    float negate = (float)rand() / (float)RAND_MAX;
    if (negate < 0.5)
        out[0] = '-';
    else
        out[0] = ' ';

    uint64_t chop_off_at = len - 1 - (rand() % (len / 2));
    out[chop_off_at] = '\0';
    out[len] = '\0';
    return out;
}
