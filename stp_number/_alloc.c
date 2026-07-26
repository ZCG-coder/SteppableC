#include "_utils.h"
#include "helpers.h"
#include "stp_number.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int _clz64(uint64_t x)
{
    if (x == 0)
        return 64;
    int n = 0;
    if ((x >> 32) == 0)
    {
        n += 32;
        x <<= 32;
    }
    if ((x >> 48) == 0)
    {
        n += 16;
        x <<= 16;
    }
    if ((x >> 56) == 0)
    {
        n += 8;
        x <<= 8;
    }
    if ((x >> 60) == 0)
    {
        n += 4;
        x <<= 4;
    }
    if ((x >> 62) == 0)
    {
        n += 2;
        x <<= 2;
    }
    if ((x >> 63) == 0)
        n += 1;
    return n;
}

uint64_t _count_digits(uint64_t x)
{
    if (x >= 10000000000000000000ULL)
        return 20;
    if (x >= 1000000000000000000ULL)
        return 19;
    if (x >= 100000000000000000ULL)
        return 18;
    if (x >= 10000000000000000ULL)
        return 17;
    if (x >= 1000000000000000ULL)
        return 16;
    if (x >= 100000000000000ULL)
        return 15;
    if (x >= 10000000000000ULL)
        return 14;
    if (x >= 1000000000000ULL)
        return 13;
    if (x >= 100000000000ULL)
        return 12;
    if (x >= 10000000000ULL)
        return 11;
    if (x >= 1000000000ULL)
        return 10;
    if (x >= 100000000ULL)
        return 9;
    if (x >= 10000000ULL)
        return 8;
    if (x >= 1000000ULL)
        return 7;
    if (x >= 100000ULL)
        return 6;
    if (x >= 10000ULL)
        return 5;
    if (x >= 1000ULL)
        return 4;
    if (x >= 100ULL)
        return 3;
    if (x >= 10ULL)
        return 2;
    return 1;
}

int _STP_Number_count_digits(const STP_Number* num)
{
    return _count_digits(num->arr[num->size - 1]) + (num->size - 1) * 19 + num->scale;
}

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
    while (num->size > 0 && num->arr[num->size - 1] == 0)
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

    if (len > src->size - start)
        len = src->size - start;

    if (!_STP_Number_ensure_capacity(dst, len))
        return 0;

    memcpy(dst->arr, src->arr + start, len * sizeof(uint64_t));
    dst->size = len;
    dst->scale = 0;
    dst->sign = 1;

    return _STP_Number_trim(dst);
}
