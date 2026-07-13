#include "_utils.h"
#include "stp_number.h"
#include "stp_string.h"

#include <assert.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main(void)
{
    srand(time(NULL));

    STP_Number n1;
    STP_Number n2;
    STP_String str;
    char* res1 = NULL;
    char* res2 = NULL;

    for (unsigned short i = 0; i < USHRT_MAX; ++i)
    {
        printf("i   = %hu\n", i);
        res1 = _generate_random_number(DEFAULT_RANDOM_LEN);
        res2 = _generate_random_number(DEFAULT_RANDOM_LEN);
        if (res1 == NULL || res2 == NULL)
            goto res_fail;

        (void)STP_Number_conv(&n1, res1);
        (void)STP_Number_conv(&n2, res2);
        STP_String_init(&str);

        if (!STP_Number_print(&n1, &str))
            goto fail;
        printf("n1  = %s\n", str.str);

        if (!STP_Number_print(&n2, &str))
            goto fail;
        printf("n2  = %s\n", str.str);

        if (!STP_Number_mul(&n1, &n2))
            goto fail;

        if (!STP_Number_print(&n1, &str))
            goto fail;
        printf("prd = %s\n", str.str);

        STP_String_destroy(&str);
        STP_Number_destroy(&n1);
        STP_Number_destroy(&n2);
        free(res1);
        free(res2);
        res1 = NULL;
        res2 = NULL;
    }

    return 0;

fail:
    STP_String_destroy(&str);
    STP_Number_destroy(&n1);
    STP_Number_destroy(&n2);

res_fail:
    free(res1);
    free(res2);
    return 1;
}
