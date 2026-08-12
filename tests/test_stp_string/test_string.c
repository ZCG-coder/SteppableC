#include "stp_string.h"

#include <stdio.h>

int main(void)
{
    STP_String str = STP_String_lit("Hello world");
    STP_String rhs = STP_String_lit("#");

    STP_String_insert(&str, 2, &rhs);
    STP_String_prepend(&str, &str);
    printf("%s\n", str.str);

    STP_String_destroy(&str);
    STP_String_destroy(&rhs);
}
