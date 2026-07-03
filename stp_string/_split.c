#include "stp_string.h"

#include <stdint.h>

int STP_String_split(STP_String* str, STP_String* tok, STP_StringSplit* res)
{
    if (str == NULL || tok == NULL || res == NULL || str->length == 0 || tok->length == 0)
    {
        res->length = 0;
        res->items = NULL;
        return 0;
    }

    uint64_t count = 0;
    uint64_t search_idx = 0;

    while (search_idx < str->length)
    {
        search_idx = STP_String_lfind(str, tok, search_idx);
        if (search_idx == UINT64_MAX)
            break;

        count++;
        search_idx += tok->length;
    }

    uint64_t total_tokens = count + 1;

    res->items = (STP_String*)malloc(sizeof(STP_String) * total_tokens);
    if (res->items == NULL)
    {
        res->length = 0;
        return 0;
    }
    res->length = total_tokens;

    for (uint64_t i = 0; i < total_tokens; ++i)
        STP_String_init(res->items + i);

    uint64_t start_pos = 0;
    uint64_t current_token = 0;
    search_idx = 0;

    while (current_token < total_tokens - 1)
    {
        search_idx = STP_String_lfind(str, tok, start_pos);

        STP_String_substr(str, res->items + current_token, start_pos, search_idx - 1);

        current_token++;
        start_pos = search_idx + tok->length;
    }

    STP_String_substr(str, res->items + current_token, start_pos, str->length - 1);

    return 1;
}

int STP_StringSplit_print(const STP_StringSplit* strsp)
{
    if (strsp == NULL)
        return 0;

    if (strsp->items == NULL || strsp->length == 0)
    {
        printf("[]\n");
        return 2;
    }

    printf("[ ");
    for (uint64_t i = 0; i < strsp->length; ++i)
    {
        STP_String* item = strsp->items + i;
        if (item == NULL)
            return 0;

        if (item->str != NULL)
            printf("\"%s\" ", item->str);
        else
            printf("\"\" ");

        if ((i + 1) % 5 == 0)
            printf("\n  ");
    }
    printf("]\n");
    return 1;
}

int STP_StringSplit_destroy(STP_StringSplit* strsp)
{
    if (strsp == NULL)
        return 0;

    for (uint64_t i = 0; i < strsp->length; ++i)
        STP_String_destroy(strsp->items + i);

    if (strsp->items != NULL)
        free(strsp->items);
    strsp->items = NULL;
    strsp->length = 0;

    return 1;
}
