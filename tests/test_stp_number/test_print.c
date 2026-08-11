#include "stp_number.h"
#include "stp_string.h"

#include <acutest.h>

void test_print(void)
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

TEST_LIST = {
    { "test_print", test_print },
    { NULL, NULL },
};
