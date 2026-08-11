#include "stp_cmplx.h"
#include "stp_string.h"

#include <acutest.h>

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

void test_cmpl_div(void)
{
    STP_Cmplx lhs;
    STP_Cmplx rhs;
    STP_Cmplx res;

    STP_String str;
    STP_String_init(&str);

    /* 127.976582888627553306132644593207529033548834131 +
     * 57240275676658342.582395593236683289673049574429815733373277920i */
    STP_Cmplx_conv(&lhs,
                   "132134351183663258724451207.134638947974487548639842140791646059595567442959041713318911572",
                   "41637751829686896046017883752146258753904121933152567.341351843783725567352641355112162447");
    STP_Cmplx_conv(&rhs,
                   "727420532788700333267315140998359263.8184318068920157012842079631556802609475496819935639",
                   "1626351253705654751135.01694913495172245800967814552179889610237183886979245867521607612069");
    STP_Cmplx_conv(&res,
                   "127.976582888627553306132644593207529033548834131",
                   "57240275676658342.582395593236683289673049574429815733373277920");

    STP_Cmplx_div(&lhs, &rhs, 45);

    STP_Cmplx_print(&lhs, &str);
    TEST_ASSERT_(STP_Cmplx_eq(&lhs, &res), "%s", str.str);

    STP_Cmplx_destroy(&lhs);
    STP_Cmplx_destroy(&rhs);
    STP_Cmplx_destroy(&res);

    STP_String_destroy(&str);
}

TEST_LIST = {
    { "test_cmpl_mul", test_cmpl_mul },
    { "test_cmpl_div", test_cmpl_div },
    { NULL, NULL },
};
