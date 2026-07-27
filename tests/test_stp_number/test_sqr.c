#include "_utils.h"
#include "deps/acutest.h"
#include "stp_number.h"

#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

void test_sqr(void)
{
    srand(time(NULL));

    STP_Number n1;
    STP_Number n2;
    char* res1 = NULL;

    for (unsigned short i = 0; i < USHRT_MAX; ++i)
    {
        res1 = _generate_random_number(DEFAULT_RANDOM_LEN);
        if (res1 == NULL)
            goto res_fail;

        (void)STP_Number_conv(&n1, res1);
        (void)STP_Number_conv(&n2, res1);

        if (!STP_Number_mul(&n1, &n1))
            goto fail;
        if (!STP_Number_sqr(&n2))
            goto fail;

        TEST_ASSERT(STP_Number_cmp(&n1, &n2) == 0);

        STP_Number_destroy(&n1);
        STP_Number_destroy(&n2);
        free(res1);
        res1 = NULL;
    }

fail:
    STP_Number_destroy(&n1);
    STP_Number_destroy(&n2);

res_fail:
    free(res1);
}

TEST_LIST = {
    { "test_sqr", test_sqr },
    { NULL, NULL },
};
