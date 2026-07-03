#include <stdio.h>
#include "stp_string.h"

int main()
{
    STP_String str = STP_String_lit("Hello world");

    STP_String rhs;
    STP_String_init(&rhs);
    STP_String_assign_buf(&rhs, "!!");
    STP_String_append(&str, &rhs);
    STP_String_prepend(&str, &rhs);
    printf("%s\n", str.str);

    STP_String_insert(&str, 2, &str);
    printf("%s\n", str.str);

    STP_String tok = STP_String_lit("!!!");
    STP_String replace = STP_String_lit("%");
    STP_String_replace(&str, &tok, &replace);
    printf("%s\n", str.str);

    STP_StringSplit strsp;
    STP_String_split(&str, &rhs, &strsp);
    STP_StringSplit_print(&strsp);

    STP_String_destroy(&str);
    STP_String_destroy(&rhs);
    STP_String_destroy(&tok);
    STP_String_destroy(&replace);
    STP_StringSplit_destroy(&strsp);
}
