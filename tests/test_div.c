#include "_utils.h"
#include "stp_number.h"
#include "stp_string.h"

#include <assert.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define DECIMALS 151

int main(void)
{
    srand(time(NULL));

    STP_Number n1;
    STP_Number n2;
    STP_String str;
    char* res1 = NULL;
    char* res2 = NULL;

    FILE* f = fopen("test_div.out.txt", "wb");
    if (f == NULL)
        return 1;

    STP_String_init(&str);

    for (unsigned short i = 0; i < USHRT_MAX; ++i)
    {
        res1 = _generate_random_number(DEFAULT_RANDOM_LEN);
        res2 = _generate_random_number(DEFAULT_RANDOM_LEN);
        if (res1 == NULL || res2 == NULL)
            goto res_fail;

        (void)STP_Number_conv(&n1, res1);
        (void)STP_Number_conv(&n2, res2);

        if (!STP_Number_print(&n1, &str))
            goto fail;
        fprintf(f, "%s\n", str.str);

        if (!STP_Number_print(&n2, &str))
            goto fail;
        fprintf(f, "%s\n", str.str);

        if (!STP_Number_div(&n1, &n2, DECIMALS))
            goto fail;

        if (!STP_Number_print(&n1, &str))
            goto fail;
        fprintf(f, "%s\n", str.str);

        STP_Number_destroy(&n1);
        STP_Number_destroy(&n2);
        free(res1);
        free(res2);
        res1 = NULL;
        res2 = NULL;
    }

    fclose(f);
    STP_String_destroy(&str);
    return 0;

fail:
    STP_String_destroy(&str);
    STP_Number_destroy(&n1);
    STP_Number_destroy(&n2);

res_fail:
    free(res1);
    free(res2);
    fclose(f);
    return 1;
}
