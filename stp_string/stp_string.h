#pragma once

#include <stdint.h>

/**
 * Dynamically-reallocating string.
 *
 * FLD length length of string
 * FLD str actual string bytes
 */
typedef struct
{
    uint64_t length;
    char* str;
} STP_String;

/**
 * Initializes str.
 *
 * ARG str unintialized string
 */
void STP_String_init(STP_String* str);

/**
 * Destroys str
 *
 * ARG str
 * REQUIRES str is initialized
 */
void STP_String_destroy(STP_String* str);

/**
 * Copies rhs to str.
 *
 * ARG str
 * ARG rhs
 * RETURNS 1 if successful, 0 otherwise
 *
 * REQUIRES rhs is already initialized
 */
int STP_String_copy(STP_String* str, const STP_String* rhs);

/**
 * Assigns buffer in rhs to str.
 *
 * ARG str
 * ARG rhs
 * RETURNS 1 if successful, 0 otherwise
 *
 * REQUIRES str and rhs are not null
 * REQUIRES str is initialized
 */
int STP_String_assign_buf(STP_String* str, const char* rhs);

/**
 * Constructs a string object from a literal.
 *
 * ARG rhs literal
 * RETURNS constructed string object
 * REQUIRES rhs is a valid string literal / char*
 */
STP_String STP_String_lit(const char* rhs);

/**
 * Appends data from rhs to str.
 *
 * ARG str
 * ARG rhs
 * RETURNS 1 if successful, 0 otherwise
 * REQUIRES str and rhs are valid strings
 */
int STP_String_append(STP_String* str, const STP_String* rhs);

/**
 * Prepends data from rhs to str
 *
 * ARG str
 * ARG rhs
 * RETURNS 1 if successful, 0 otherwise
 * REQUIRES str and rhs are valid strings
 */
int STP_String_prepend(STP_String* str, const STP_String* rhs);

/**
 * Inserts rhs to str, at index of where
 *
 * ARG str
 * ARG where where to insert the string
 * ARG rhs
 *
 * RETURNS 1 if successful, 0 otherwise
 * REQUIRES str and rhs are valid strings
 * REQUIRES where < str->length
 */
int STP_String_insert(STP_String* str, uint64_t where, const STP_String* rhs);

/**
 * Checks if rhs is the same as str.
 *
 * ARG str
 * ARG rhs
 * RETURNS 1 if equal, 0 otherwise. An arbitrary value if error occurs.
 *
 * REQUIRES str and rhs are valid strings
 */
int STP_String_eq(STP_String* str, const STP_String* rhs);

/**
 * Removes a section from str
 *
 * ARG str
 * ARG start start of section to remove (inclusive)
 * ARG end end of section to remove (inclusive)
 *
 * RETURNS 1 if successful, 0 otherwise
 * REQUIRES str and rhs are valid strings
 * REQUIRES start < end < str->length
 */
int STP_String_remove(STP_String* str, uint64_t start, uint64_t end);

/**
 * Trim away trailing spaces in str
 *
 * ARG str
 * RETURNS 1 if successful, 0 otherwise
 */
int STP_String_rtrim(STP_String* str);

/**
 * Trim away leading spaces in str
 *
 * ARG str
 * RETURNS 1 if successful, 0 otherwise
 */
int STP_String_ltrim(STP_String* str);

/**
 * Trim away spaces from both ends in str
 *
 * ARG str
 * RETURNS 1 if successful, 0 otherwise
 */
inline int STP_String_trim(STP_String* str) { return STP_String_ltrim(str) && STP_String_rtrim(str); }

/**
 * Create a substring from a section from str
 *
 * ARG str
 * ARG res pointer to string where substring is written
 * ARG start start of section to copy (inclusive)
 * ARG end end of section to copy (inclusive)
 *
 * RETURNS 1 if successful, 0 otherwise
 * REQUIRES str and res are valid, initialized strings
 * REQUIRES start < end < str->length
 */
int STP_String_substr(STP_String* str, STP_String* res, uint64_t start, uint64_t end);

/**
 * Find a substring find in str, starting at start.
 *
 * ARG str
 * ARG find substring to find
 * ARG start
 * RETURNS index to first character of first occurrence of find. UINT64_MAX if not found.
 *
 * REQUIRES str and find are all initialized, valid strings.
 */
uint64_t STP_String_lfind(STP_String* str, STP_String* find, uint64_t start);

/**
 * Find a substring find in str, starting at end.
 *
 * ARG str
 * ARG find substring to find
 * ARG end
 * RETURNS index to first character of last occurrence of find. UINT64_MAX if not found.
 *
 * REQUIRES str and find are all initialized, valid strings.
 */
uint64_t STP_String_rfind(STP_String* str, STP_String* find, uint64_t end);

/**
 * Replace all occurrences of find with replace in str.
 *
 * ARG str
 * ARG find substring to find
 * ARG replace string to replace find
 * RETURNS number of substrings replaced
 *
 * REQUIRES str, find and replace are all initialized, valid strings.
 */
uint64_t STP_String_replace(STP_String* str, STP_String* find, STP_String* replace);

/**
 * Convert each character to lowercase in str.
 *
 * ARG str
 * RETURNS 1 if successful, 0 otherwise
 *
 * REQUIRES str is a valid string.
 */
int STP_String_tolower(STP_String* str);

/**
 * Convert each character to uppercase in str.
 *
 * ARG str
 * RETURNS 1 if successful, 0 otherwise
 *
 * REQUIRES str is a valid string.
 */
int STP_String_toupper(STP_String* str);

/**
 * Get length of str.
 *
 * ARG str
 * RETURNS length of str
 *
 * REQUIRES str is a valid string.
 */
inline uint64_t STP_String_length(const STP_String* str) { return str->length; }

/* String split operations */

/**
 * Split string
 *
 * FLD length number of chunks the string is split into
 * FLD items chunks
 */
typedef struct
{
    uint64_t length;
    STP_String* items;
} STP_StringSplit;

/**
 * Split str into chunks separated by tok.
 *
 * ARG str
 * ARG tok token
 * ARG res where result is written to
 * RETURNS 1 if successful, 0 otherwise.
 */
int STP_String_split(STP_String* str, STP_String* tok, STP_StringSplit* res);

/**
 * Print out chunks in strsp, for debugging.
 *
 * ARG strsp STP_StringSplit object written.
 * RETURNS 1 if successful, 0 otherwise.
 */
int STP_StringSplit_print(const STP_StringSplit* strsp);

/**
 * Destroys strsp to free up memory.
 *
 * ARG strsp STP_StringSplit object written.
 * RETURNS 1 if successful, 0 otherwise.
 */
int STP_StringSplit_destroy(STP_StringSplit* strsp);
