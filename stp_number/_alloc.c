#include "_utils.h"
#include "helpers.h"
#include "stp_number.h"

#include <stdint.h>

int _STP_Number_ensure_capacity(STP_Number* num, uint64_t min_capacity)
{
    if (num == NULL || num->arr == NULL)
        return 0;

    if (num->capacity >= min_capacity)
        return 1;

    /* Exponential growth, ensures O(1) performance slowdown */
    uint64_t new_capacity = num->capacity * 2;
    if (new_capacity < min_capacity)
        new_capacity = min_capacity;

    uint64_t* new_arr = realloc(num->arr, new_capacity * sizeof(uint64_t));
    if (new_arr == NULL)
    {
        fprintf(stderr, "%s: realloc failed\n", STP_CURRENT_FUNCTION);
        return 0;
    }
    uint64_t diff = new_capacity - num->capacity;
    memset(new_arr + num->capacity, 0, diff * sizeof(uint64_t));

    num->arr = new_arr;
    num->capacity = new_capacity;
    return 1;
}

int _STP_Number_trim(STP_Number* num)
{
    while (num->size > 1 && num->arr[num->size - 1] == 0)
        num->size--;

    return 1;
}
