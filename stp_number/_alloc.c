#include "_utils.h"
#include "helpers.h"
#include "stp_number.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

int _STP_Number_slice(STP_Number* dst, const STP_Number* src, uint64_t start, uint64_t len)
{
    if (dst == NULL || src == NULL)
        return 0;

    if (start >= src->size)
    {
        dst->size = 0;
        dst->scale = 0;
        dst->sign = 1;
        return 1;
    }

    if (start + len > src->size)
        len = src->size - start;

    if (!_STP_Number_ensure_capacity(dst, len))
        return 0;

    memcpy(dst->arr, src->arr + start, len * sizeof(uint64_t));
    dst->size = len;
    dst->scale = 0;
    dst->sign = 1;

    return _STP_Number_trim(dst);
}

uint64_t _STP_Number_bit_count(STP_Number* num)
{
    if (num == NULL || num->arr == NULL)
        return 0;

    if (!_STP_Number_trim(num))
        return 0;

    uint64_t block_cnt = num->size;
    uint64_t highest_block = num->arr[block_cnt - 1];
    uint64_t occupied_bits = 0;

    while (highest_block > 0)
    {
        highest_block >>= 1;
        ++occupied_bits;
    }

    uint64_t bit_cnt = (block_cnt - 1) * 64 + occupied_bits;
    return bit_cnt;
}
