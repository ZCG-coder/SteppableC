#include "_utils.h"
#include "stp_number.h"

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
