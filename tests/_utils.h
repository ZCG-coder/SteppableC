#pragma once

#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>

#define DEFAULT_RANDOM_LEN 152

char* _generate_random_number(uint64_t len)
{
    if (len == 0)
        return NULL;

    char* out = malloc((len + 1) * sizeof(char)); /* +1 for NULL */
    if (out == NULL)
        return NULL;

    int placed_decimal = 0;

    for (uint64_t i = 0; i < len; ++i)
    {
        if (!placed_decimal && i != 0)
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

    uint64_t chop_off_at = len - 1 - (rand() % (len / 2));
    out[chop_off_at] = '\0';
    out[len] = '\0';
    return out;
}
