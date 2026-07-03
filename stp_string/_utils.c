#include "_utils.h"
#include "helpers.h"

#include <stdlib.h>
#include <stdint.h>

int _STP_String_realloc(char** p_str, uint64_t new_str_len)
{
    char* new_ptr = (char*)realloc(*p_str, new_str_len + 1);
    if (new_ptr == NULL)
    {
        fprintf(stderr, "%s: realloc failed", STP_CURRENT_FUNCTION);
        return 0;
    }
    *p_str = new_ptr;
    return 1;
}
