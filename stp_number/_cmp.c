#include "helpers.h"
#include "stp_number.h"

#include <stdint.h>
#include <string.h>

int STP_Number_is_zero(const STP_Number* num)
{
    if (num == NULL || num->arr == NULL)
    {
        fprintf(stderr, "%s: Invalid number input\n", STP_CURRENT_FUNCTION);
        return 0;
    }
    if (num->size == 0)
        return 1;

    for (uint64_t idx = 0; idx < num->size; ++idx)
        if (num->arr[idx] != 0)
            return 0;

    return 1;
}
