#include "_utils.h"
#include "config.h"
#include "helpers.h"
#include "stp_number.h"

#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int _clz64(uint64_t x)
{
    if (x == 0)
        return 64;

#if HAS_CLZLL
    return __builtin_clzll(x);
#else
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
#endif
}

uint64_t _count_digits(uint64_t x)
{
    if (x >= 10000000000000000000ULL) /* 10^19 */
        return 20;
    if (x >= 1000000000000000000ULL) /* 10^18 */
        return 19;
    if (x >= 100000000000000000ULL) /* 10^17 */
        return 18;
    if (x >= 10000000000000000ULL) /* 10^16 */
        return 17;
    if (x >= 1000000000000000ULL) /* 10^15 */
        return 16;
    if (x >= 100000000000000ULL) /* 10^14 */
        return 15;
    if (x >= 10000000000000ULL) /* 10^13 */
        return 14;
    if (x >= 1000000000000ULL) /* 10^12 */
        return 13;
    if (x >= 100000000000ULL) /* 10^11 */
        return 12;
    if (x >= 10000000000ULL) /* 10^10 */
        return 11;
    if (x >= 1000000000ULL) /* 10^9 */
        return 10;
    if (x >= 100000000ULL) /* 10^8 */
        return 9;
    if (x >= 10000000ULL) /* 10^7 */
        return 8;
    if (x >= 1000000ULL) /* 10^6 */
        return 7;
    if (x >= 100000ULL) /* 10^5 */
        return 6;
    if (x >= 10000ULL) /* 10^4 */
        return 5;
    if (x >= 1000ULL) /* 10^3 */
        return 4;
    if (x >= 100ULL) /* 10^2 */
        return 3;
    if (x >= 10ULL) /* 10 */
        return 2;
    return 1; /* 1 */
}

uint64_t _first_digit(uint64_t x)
{
    if (x >= 10000000000000000000ULL) /* 10^19 */
        return x / 10000000000000000000ULL;
    if (x >= 1000000000000000000ULL) /* 10^18 */
        return x / 1000000000000000000ULL;
    if (x >= 100000000000000000ULL) /* 10^17 */
        return x / 100000000000000000ULL;
    if (x >= 10000000000000000ULL) /* 10^16 */
        return x / 10000000000000000ULL;
    if (x >= 1000000000000000ULL) /* 10^15 */
        return x / 1000000000000000ULL;
    if (x >= 100000000000000ULL) /* 10^14 */
        return x / 100000000000000ULL;
    if (x >= 10000000000000ULL) /* 10^13 */
        return x / 10000000000000ULL;
    if (x >= 1000000000000ULL) /* 10^12 */
        return x / 1000000000000ULL;
    if (x >= 100000000000ULL) /* 10^11 */
        return x / 100000000000ULL;
    if (x >= 10000000000ULL) /* 10^10 */
        return x / 10000000000ULL;
    if (x >= 1000000000ULL) /* 10^9 */
        return x / 1000000000ULL;
    if (x >= 100000000ULL) /* 10^8 */
        return x / 100000000ULL;
    if (x >= 10000000ULL) /* 10^7 */
        return x / 10000000ULL;
    if (x >= 1000000ULL) /* 10^6 */
        return x / 1000000ULL;
    if (x >= 100000ULL) /* 10^5 */
        return x / 100000ULL;
    if (x >= 10000ULL) /* 10^4 */
        return x / 10000ULL;
    if (x >= 1000ULL) /* 10^3 */
        return x / 1000ULL;
    if (x >= 100ULL) /* 10^2 */
        return x / 100ULL;
    if (x >= 10ULL) /* 10 */
        return x / 10ULL;
    return x; /* 1 */
}

int64_t _STP_Number_int_digits(const STP_Number* num)
{
    return _count_digits(num->arr[num->size - 1]) + (num->size - 1) * 19 + num->scale;
}

int64_t _STP_Number_sig_digits(const STP_Number* num)
{
    return (int64_t)(num->size - 1) * 19 + (int64_t)_count_digits(num->arr[num->size - 1]);
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
        STP_ERRMSG(STP_CURRENT_FUNCTION, errno);
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
