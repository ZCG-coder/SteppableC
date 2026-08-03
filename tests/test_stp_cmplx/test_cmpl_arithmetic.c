#include "deps/acutest.h"
#include "stp_cmplx.h"

void test_cmpl_mul(void)
{
    STP_Cmplx lhs;
    STP_Cmplx rhs;
    STP_Cmplx res;

    /* 132663414762025930.260822811923531347 + 467210648051435378.391385510935470947i */
    STP_Cmplx_conv(&lhs, "894183189.821534599", "210121876.574721229");
    STP_Cmplx_conv(&rhs, "256954812.994133598", "462118752.906407995");
    STP_Cmplx_conv(&res, "132663414762025930.260822811923531347", "467210648051435378.391385510935470947");

    STP_Cmplx_mul(&lhs, &rhs);

    TEST_ASSERT(STP_Cmplx_eq(&lhs, &res));

    STP_Cmplx_destroy(&lhs);
    STP_Cmplx_destroy(&rhs);
    STP_Cmplx_destroy(&res);
}

TEST_LIST = {
    { "test_cmpl_mul", test_cmpl_mul },
    { NULL, NULL },
};