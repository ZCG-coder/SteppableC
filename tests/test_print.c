#include "deps/acutest.h"
#include "stp_number.h"
#include "stp_string.h"

void test_print(void)
{
    const char* string = " 866919200990534219744984542.950135983558795406721665005";
    STP_Number num;
    STP_Number_conv(&num, string);

    STP_String str;
    STP_String_init(&str);
    STP_Number_print(&num, &str);

    TEST_ASSERT(strcmp(str.str, string) == 0);

    STP_String_destroy(&str);
    STP_Number_destroy(&num);
}

TEST_LIST = {
    { "test_print", test_print },
    { NULL, NULL },
};
