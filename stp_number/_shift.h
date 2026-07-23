#pragma once
#include <stdint.h>

uint64_t _raw_shift_left(uint64_t* blocks, uint64_t size, uint32_t bits);

void _raw_shift_right(uint64_t* blocks, uint64_t size, uint32_t bits);
