#pragma once

#include <stdint.h>

/**
 * Reallocates memory for string.
 *
 * ARG p_str pointer to string to be resized
 * ARG new_str_len
 * RETURNS 1 if successful, 0 otherwise
 *
 * MODIFIES p_str to increase in size to new_str_len + 1. May change pointer.
 */
int _STP_String_realloc(char** p_str, uint64_t new_str_len);

#define _STP_STRING_REALLOC_S(p_str, len)                   \
    do                                                      \
    {                                                       \
        if (_STP_String_realloc(&(p_str->str), (len)) == 0) \
            return 0;                                       \
    } while (0);
