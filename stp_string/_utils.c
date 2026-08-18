#include "_utils.h"

#include "helpers.h"
#include "stp_string.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

int _STP_String_realloc(STP_String* p_str, uint64_t new_str_len)
{
    if (p_str->capacity >= new_str_len)
        return 1;

    uint64_t capacity = new_str_len * 2 + 1;
    char* new_ptr = (char*)realloc(p_str->str, capacity);
    if (new_ptr == NULL)
    {
        fprintf(stderr, "%s: realloc failed\n", STP_CURRENT_FUNCTION);
        STP_ERRMSG(STP_CURRENT_FUNCTION, errno);
        return 0;
    }
    p_str->str = new_ptr;
    p_str->capacity = capacity;
    return 1;
}
