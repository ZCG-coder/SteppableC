#pragma once

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct
{
    uint64_t length;
    char* str;
} STP_String;

void STP_String_init(STP_String* str);

void STP_String_destroy(STP_String* str);

int STP_String_copy(STP_String* str, const STP_String* rhs);

int STP_String_assign_buf(STP_String* str, const char* rhs);

STP_String STP_String_lit(const char* rhs);

int STP_String_append(STP_String* str, const STP_String* rhs);

int STP_String_prepend(STP_String* str, const STP_String* rhs);

int STP_String_insert(STP_String* str, uint64_t where, const STP_String* rhs);

int STP_String_eq(STP_String* str, const STP_String* rhs);

int STP_String_remove(STP_String* str, uint64_t start, uint64_t end);

int STP_String_rtrim(STP_String* str);

int STP_String_ltrim(STP_String* str);

inline int STP_String_trim(STP_String* str)
{
    return STP_String_ltrim(str) && STP_String_rtrim(str);
}

int STP_String_substr(STP_String* str, STP_String* res, uint64_t start, uint64_t end);

uint64_t STP_String_lfind(STP_String* str, STP_String* find, uint64_t start);

uint64_t STP_String_rfind(STP_String* str, STP_String* find, uint64_t end);

uint64_t STP_String_replace(STP_String* str, STP_String* find, STP_String* replace);

int STP_String_tolower(STP_String* str);

int STP_String_toupper(STP_String* str);

inline uint64_t STP_String_length(const STP_String *str)
{
    return str->length;
}

/* String split operations */

typedef struct
{
    uint64_t length;
    STP_String* items;
} STP_StringSplit;

int STP_String_split(STP_String* str, STP_String* tok, STP_StringSplit* res);

int STP_StringSplit_print(const STP_StringSplit* strsp);

int STP_StringSplit_destroy(STP_StringSplit* strsp);
