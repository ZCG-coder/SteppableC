#include "_utils.h"
#include "deps/acutest.h"
#include "stp_number.h"
#include "stp_string.h"

#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

void test_exp(void)
{
    STP_Number n1;
    STP_Number n2;

    (void)STP_Number_conv(&n1, "500");
    (void)STP_Number_conv(
        &n2,
        "14035922178528374107397703328409120821806021155655454250255643688895552313943821922640079350083432091928424275"
        "200106921063850971260135477134602885117174978399377774315602187475245307510202478280037493197625688184439170."
        "92385517766798217100267638607046412149763067370417662828830278999461466402166601077281845642576861331889755511"
        "781962166652995998098147863957197170783796571893294115785689810061183663432395764256573372");

    if (!STP_Number_exp(&n1, 200))
        goto fail;

    TEST_ASSERT(STP_Number_cmp(&n1, &n2) == 0);

fail:
    STP_Number_destroy(&n1);
    STP_Number_destroy(&n2);
}

TEST_LIST = {
    { "test_exp", test_exp },
    { NULL, NULL },
};
