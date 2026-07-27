#include "_utils.h"
#include "stp_number.h"

#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main(void)
{
    srand(time(NULL));

    STP_Number n1;
    STP_String str;
    char* res1 = NULL;

    FILE* f = fopen("test_self_div.out.txt", "wb");
    if (f == NULL)
        return 1;

    STP_String_init(&str);

    for (unsigned short i = 0; i < USHRT_MAX; ++i)
    {
        res1 = _generate_random_number(DEFAULT_RANDOM_LEN);
        if (res1 == NULL)
            goto res_fail;

        (void)STP_Number_conv(&n1, res1);

        if (!STP_Number_print(&n1, &str))
            goto fail;
        fprintf(f, "%s\n", str.str);
        fprintf(f, "%s\n", str.str);

        if (!STP_Number_div(&n1, &n1, 10))
            goto fail;

        if (!STP_Number_print(&n1, &str))
            goto fail;
        fprintf(f, "%s\n", str.str);

        STP_Number_destroy(&n1);
        free(res1);
        res1 = NULL;
    }

    fclose(f);
    STP_String_destroy(&str);
    return 0;

fail:
    STP_String_destroy(&str);
    STP_Number_destroy(&n1);

res_fail:
    free(res1);
    fclose(f);
    return 1;
}
