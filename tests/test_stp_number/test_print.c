#include "stp_number.h"
#include "stp_string.h"

#include <acutest.h>

void test_print_decimal(void)
{
    const char* string = " 866919200990534219744984542.95013598";
    STP_Number num;
    STP_Number_conv(&num, string);

    STP_String str;
    STP_String_init(&str);
    STP_Number_print(&num, &str);

    TEST_ASSERT_(strcmp(str.str, string) == 0, "Got %s", str.str);

    STP_String_destroy(&str);
    STP_Number_destroy(&num);
}

void test_print_int(void)
{
    char* string = " 866919200990534219744984542.";
    char* string2 = " 86691920099053421974498454200000000000000.";

    STP_Number num;
    STP_Number_conv(&num, string);
    num.scale += 14;

    STP_String str;
    STP_String_init(&str);
    STP_Number_print(&num, &str);

    TEST_ASSERT_(strcmp(str.str, string2) == 0, "Got '%s'", str.str);

    STP_String_destroy(&str);
    STP_Number_destroy(&num);
}

void test_print_smallfrac(void)
{
    char* string = " 1.";
    char* string2 = " 0.0001";

    STP_Number num;
    STP_Number_conv(&num, string);
    num.scale -= 4;

    STP_String str;
    STP_String_init(&str);
    STP_Number_print(&num, &str);

    TEST_ASSERT_(strcmp(str.str, string2) == 0, "Got '%s'", str.str);

    STP_String_destroy(&str);
    STP_Number_destroy(&num);
}

TEST_LIST = {
    { "test_print_decimal", test_print_decimal },
    { "test_print_smallfrac", test_print_smallfrac },
    { "test_print_int", test_print_int },
    { NULL, NULL },
};
