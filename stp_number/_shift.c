#include "_utils.h"
#include "stp_number.h"

#include <string.h>

uint64_t _raw_shift_left(uint64_t* blocks, uint64_t size, uint32_t bits)
{
    if (bits == 0 || size == 0)
        return 0;

    uint32_t carry_shift = 64 - bits;
    uint64_t carry = 0;

    for (uint64_t i = 0; i < size; i++)
    {
        uint64_t next_carry = blocks[i] >> carry_shift;
        blocks[i] = (blocks[i] << bits) | carry;
        carry = next_carry;
    }
    return carry;
}

void _raw_shift_right(uint64_t* blocks, uint64_t size, uint32_t bits)
{
    if (bits == 0 || size == 0)
        return;

    uint32_t carry_shift = 64 - bits;
    uint64_t carry = 0;

    for (uint64_t i = size; i > 0; i--)
    {
        uint64_t idx = i - 1;
        uint64_t next_carry = blocks[idx] << carry_shift;
        blocks[idx] = (blocks[idx] >> bits) | carry;
        carry = next_carry;
    }
}

int _STP_Number_lshift_blocks(STP_Number* num, uint64_t blocks)
{
    if (num == NULL)
        return 0;

    if (blocks == 0 || num->size == 0)
        return 1;

    if (!_STP_Number_ensure_capacity(num, num->size + blocks))
        return 0;

    memmove(num->arr + blocks, num->arr, num->size * sizeof(uint64_t));
    memset(num->arr, 0, blocks * sizeof(uint64_t));

    num->size += blocks;
    return 1;
}

int STP_Number_lshift(STP_Number* num, uint64_t bits)
{
    if (num == NULL)
        return 0;
    if (bits == 0 || num->size == 0)
        return 1;

    uint64_t block_shift = bits / 64;
    uint32_t residual_bits = (uint32_t)(bits % 64);

    if (block_shift > 0)
    {
        if (!_STP_Number_lshift_blocks(num, block_shift))
            return 0;
    }

    if (residual_bits > 0)
    {
        if (!_STP_Number_ensure_capacity(num, num->size + 1))
            return 0;

        uint64_t carry = _raw_shift_left(num->arr, num->size, residual_bits);
        if (carry > 0)
        {
            num->arr[num->size] = carry;
            num->size++;
        }
    }
    return 1;
}

int STP_Number_rshift(STP_Number* num, uint64_t bits)
{
    if (num == NULL)
        return 0;
    if (bits == 0 || num->size == 0)
        return 1;

    uint64_t block_shift = bits / 64;
    uint32_t residual_bits = (uint32_t)(bits % 64);

    if (block_shift >= num->size)
    {
        STP_Number_clear(num);
        return 1;
    }

    if (block_shift > 0)
    {
        uint64_t new_size = num->size - block_shift;
        memmove(num->arr, num->arr + block_shift, new_size * sizeof(uint64_t));
        num->size = new_size;
    }

    if (residual_bits > 0)
        _raw_shift_right(num->arr, num->size, residual_bits);

    return _STP_Number_trim(num);
}
