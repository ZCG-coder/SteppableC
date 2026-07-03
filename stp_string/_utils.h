#pragma once

#include "helpers.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

int _STP_String_realloc(char** p_str, uint64_t new_str_len);

#define _STP_STRING_REALLOC_S(p_str, len)                   \
    do                                                      \
    {                                                       \
        if (_STP_String_realloc(&(p_str->str), (len)) == 0) \
            return 0;                                       \
    } while (0);
