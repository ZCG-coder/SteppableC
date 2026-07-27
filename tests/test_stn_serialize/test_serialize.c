#include "deps/acutest.h"
#include "stn_serialize.h"
#include "stp_number.h"

#define SERIALIZE_N                                                                                                  \
    "438303674295840512472859576720."                                                                                \
    "48496026563813735548358480840195880282393567196701669789498838466924280674125621444202893815078610951516901810" \
    "33396010571100140626305436963522688574449105486260291023352158481535245494264793265689069334577857965964917544" \
    "09021984641330348441214786601966714045188617342418624254346832253746068471299723099996096446838657648399841793" \
    "81357168175912064721191454496208810973586681950126929580130778452627203"

#define SER_FILE "test_serialize.out.stn"

void test_serialize_basic(void)
{
    STP_Number n;
    STP_Number_conv(&n, SERIALIZE_N);

    STP_Number_serialize((STP_Number*){ &n }, 1, SER_FILE);

    STP_Number_destroy(&n);
}

void test_deserialize_basic(void)
{
    STP_Number n;
    STP_Number_deserialize(SER_FILE, &n, 1);

    STP_Number n2;
    STP_Number_conv(&n2, SERIALIZE_N);

    TEST_ASSERT(STP_Number_cmp(&n, &n2) == 0);

    STP_Number_destroy(&n2);
    STP_Number_destroy(&n);
}

TEST_LIST = {
    { "test_serialize_basic", test_serialize_basic },
    { "test_deserialize_basic", test_deserialize_basic },
    { NULL, NULL },
};
