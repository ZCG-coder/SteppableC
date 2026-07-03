#include "_utils.h"
#include "helpers.h"
#include "stp_string.h"

#include <complex.h>
#include <ctype.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void STP_String_init(STP_String* str)
{
    str->length = 0;
    str->str = NULL;
}

void STP_String_destroy(STP_String* str)
{
    str->length = 0;
    free(str->str);
    str->str = NULL;
}

int STP_String_copy(STP_String* str, const STP_String* rhs)
{
    if (rhs == str)
        return 1;

    _STP_STRING_REALLOC_S(str, rhs->length);
    str->length = rhs->length;
    memmove(str->str, rhs->str, rhs->length);
    str->str[str->length] = '\0';

    return 1;
}

int STP_String_assign_buf(STP_String* str, const char* rhs)
{
    if (rhs == NULL)
        return 0;

    str->length = strlen(rhs);
    _STP_STRING_REALLOC_S(str, str->length);
    memmove(str->str, rhs, str->length);
    str->str[str->length] = '\0';

    return 1;
}

STP_String STP_String_lit(const char* rhs)
{
    STP_String str;
    STP_String_init(&str);
    STP_String_assign_buf(&str, rhs);

    return str;
}

int STP_String_append(STP_String* str, const STP_String* rhs)
{
    if (rhs == NULL || rhs->length == 0)
        return 1;

    uint64_t old_length = str->length;
    uint64_t new_length = old_length + rhs->length;

    _STP_STRING_REALLOC_S(str, new_length);

    if (str != rhs)
        memmove(str->str + old_length, rhs->str, rhs->length);
    else
        memmove(str->str + old_length, str->str, old_length);
    str->length += rhs->length;
    str->str[str->length] = '\0';
    return 1;
}

int STP_String_prepend(STP_String* str, const STP_String* rhs)
{
    if (rhs == NULL || rhs->length == 0)
        return 1;

    uint64_t old_length = str->length;
    uint64_t new_length = old_length + rhs->length;

    _STP_STRING_REALLOC_S(str, new_length);
    memmove(str->str + rhs->length, str->str, old_length);
    memmove(str->str, rhs->str, rhs->length);
    str->length = new_length;
    str->str[new_length] = '\0';

    return 1;
}

int STP_String_insert(STP_String* str, uint64_t where, const STP_String* rhs)
{
    if (rhs == NULL || rhs->length == 0)
        return 1;

    uint64_t old_length = str->length;
    uint64_t new_length = old_length + rhs->length;
    uint64_t rhs_length = rhs->length;

    if (where > old_length)
        return 0;
    if (where == 0)
        return STP_String_prepend(str, rhs);
    if (where == old_length)
        return STP_String_append(str, rhs);

    if (rhs != str)
    {
        _STP_STRING_REALLOC_S(str, new_length);
        memmove(str->str + where + old_length, str->str + where, old_length - where);
        memmove(str->str + where, rhs->str, rhs_length);
    }
    else
    {
        char* temp_rhs = (char*)malloc(rhs_length);
        if (_STP_String_realloc(&str->str, new_length) == 0)
        {
            free(temp_rhs);
            return 0;
        }
        if (temp_rhs == NULL)
            return 0;
        memcpy(temp_rhs, str->str, rhs_length);
        memmove(str->str + where + rhs_length, str->str + where, old_length - where);
        memcpy(str->str + where, temp_rhs, rhs_length);
        free(temp_rhs);
    }
    str->length = new_length;
    str->str[new_length] = '\0';

    return 1;
}

int STP_String_eq(STP_String* str, const STP_String* rhs)
{
    if (str->str == rhs->str)
    {
        /* Self must be equal to self */
        return 1;
    }
    if (str == NULL || rhs == NULL)
        return (str == rhs);

    if (str->length != rhs->length)
    {
        /* Unequal lengths must mean unequal texts */
        return 0;
    }

    return memcmp(str->str, rhs->str, str->length) == 0;
}

int STP_String_remove(STP_String* str, uint64_t start, uint64_t end)
{
    if (end >= str->length || end < start)
        return 0;

    uint64_t size_to_remove = end - start + 1;
    uint64_t tail_size = str->length - end;
    uint64_t new_size = str->length - size_to_remove;

    memmove(str->str + start, str->str + end + 1, tail_size);
    str->length = new_size;
    str->str[str->length] = '\0';
    _STP_STRING_REALLOC_S(str, new_size);

    return 1;
}

int STP_String_rtrim(STP_String* str)
{
    if (str->length == 0)
        return 1;

    uint64_t idx = str->length - 1;
    while (idx > 0 && isspace((unsigned char)str->str[idx]))
        --idx;

    if (idx == 0 && isspace((unsigned char)str->str[0]))
        str->length = 0;
    else
        str->length = idx + 1;
    str->str[str->length] = '\0';
    _STP_STRING_REALLOC_S(str, str->length);

    return 1;
}

int STP_String_ltrim(STP_String* str)
{
    if (str->length == 0)
        return 1;

    uint64_t idx = 0;
    while (isspace((unsigned char)str->str[idx]))
        idx++;

    if (idx == 0)
        return 1;

    uint64_t new_len = str->length - idx;
    memmove(str->str, str->str + idx, new_len + 1);
    str->length = new_len;
    _STP_STRING_REALLOC_S(str, new_len);
    return 1;
}

int STP_String_substr(STP_String* str, STP_String* res, uint64_t start, uint64_t end)
{
    if (str->length == 0)
        return 0;

    if (start > end)
        return 0;
    if (end >= str->length)
        return 0;

    uint64_t new_size = end - start + 1;
    if (res == str) /* In-place crop */
    {
        memmove(str->str, str->str + start, new_size);
        str->length = new_size;
        str->str[str->length] = '\0';
        _STP_STRING_REALLOC_S(str, new_size);
        return 1;
    }

    _STP_STRING_REALLOC_S(res, new_size);
    res->length = new_size;
    memmove(res->str, str->str + start, new_size);
    res->str[new_size] = '\0';
    return 1;
}

uint64_t STP_String_lfind(STP_String* str, STP_String* find, uint64_t start)
{
    if (str->length < find->length)
        return UINT64_MAX;

    if (str->length == 0 || find->length == 0)
        return UINT64_MAX;

    if (start >= str->length)
        return UINT64_MAX;

    for (uint64_t idx = start; idx <= str->length - find->length; ++idx)
        if (str->str[idx] == find->str[0] && memcmp(str->str + idx, find->str, find->length) == 0)
            return idx;

    return UINT64_MAX;
}

uint64_t STP_String_rfind(STP_String* str, STP_String* find, uint64_t end)
{
    if (str == NULL || find == NULL)
        return UINT64_MAX;

    if (str->length < find->length)
        return UINT64_MAX;

    if (str->length == 0 || find->length == 0)
        return UINT64_MAX;

    if (end > str->length - find->length)
        return UINT64_MAX;

    for (uint64_t j = end + 1; j > 0; --j)
    {
        uint64_t idx = j - 1;

        if (str->str[idx] == find->str[0] && memcmp(str->str + idx, find->str, find->length) == 0)
            return idx;
    }

    return UINT64_MAX;
}

uint64_t STP_String_replace(STP_String* str, STP_String* find, STP_String* replace)
{
    if (str == NULL || find == NULL || replace == NULL)
        return 0;

    if (str->length == 0 || find->length == 0)
        return 0;

    uint64_t j = 0;
    uint64_t c = 0;
    uint64_t last_pos = 0;

    STP_String new_str;
    STP_String substr;
    STP_String_init(&new_str);
    STP_String_init(&substr);

    while (j < str->length)
    {
        j = STP_String_lfind(str, find, j);
        if (j == UINT64_MAX)
            break;

        if (j > last_pos)
        {
            STP_String_substr(str, &substr, last_pos, j - 1);
            STP_String_append(&new_str, &substr);
        }

        STP_String_append(&new_str, replace);
        ++c;

        j += find->length;
        last_pos = j;
    }

    if (last_pos < str->length)
    {
        STP_String_substr(str, &substr, last_pos, str->length - 1);
        STP_String_append(&new_str, &substr);
    }

    STP_String_destroy(str);
    STP_String_destroy(&substr);
    *str = new_str;

    return c;
}

int STP_String_toupper(STP_String* str)
{
    for (uint64_t i = 0; i < str->length; ++i)
        str->str[i] = toupper((unsigned char)str->str[i]);

    return 1;
}


int STP_String_tolower(STP_String* str)
{
    for (uint64_t i = 0; i < str->length; ++i)
        str->str[i] = tolower((unsigned char)str->str[i]);

    return 1;
}
