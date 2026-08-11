#include "stn_serialize.h"
#include "stp_number.h"
#include "stp_string.h"

#include <acutest.h>
#include <stdint.h>

#define SERIALIZE_N1                                                                                                 \
    "438303674295840512472859576720."                                                                                \
    "48496026563813735548358480840195880282393567196701669789498838466924280674125621444202893815078610951516901810" \
    "33396010571100140626305436963522688574449105486260291023352158481535245494264793265689069334577857965964917544" \
    "09021984641330348441214786601966714045188617342418624254346832253746068471299723099996096446838657648399841793" \
    "81357168175912064721191454496208810973586681950126929580130778452627203"
#define SERIALIZE_N2                                                                                                   \
    "7960811778871257156068893249696358587764986446692281763165287768624595104379046739339798888949886615588971546108" \
    "2479635196059945236416990728785929960276723965653121959258227134493438167429335533735920688306704749516222379243" \
    "4214339307777443768491325217587625061803408564060425745917110945775125897205470550347208058380261227868632082203" \
    "3009598516037443070702206451429387438348104596699644538891104307988752774310593789274896033300289032385442946175" \
    "45996733558328990394289495701323720123039440675887822859."                                                        \
    "9496770271061013260533791784650714327726121331900424275271052731740808716594464118421311645149500472254502713603" \
    "76603141691547358827239983303620740910787161309978465578150508712496296830556"

#define SER_FILE "test_serialize.out.stn"

void test_serialize_basic(void)
{
    STP_Number n1;
    STP_Number_conv(&n1, SERIALIZE_N1);
    STP_Number n2;
    STP_Number_conv(&n2, SERIALIZE_N2);

    STP_Number_serialize((STP_Number[]){ n1, n2 }, 2, SER_FILE);

    STP_Number_destroy(&n1);
    STP_Number_destroy(&n2);
}

void test_deserialize_basic(void)
{
    STP_Number ns[2];
    STP_Number_deserialize(SER_FILE, ns, 2);

    STP_String str;
    STP_String_init(&str);

    STP_Number t1;
    STP_Number_conv(&t1, SERIALIZE_N1);
    STP_Number t2;
    STP_Number_conv(&t2, SERIALIZE_N2);

    STP_Number_print(&ns[0], &str);
    TEST_ASSERT_(STP_Number_cmp(&ns[0], &t1) == 0, "%s", str.str);

    STP_Number_print(&ns[1], &str);
    TEST_ASSERT_(STP_Number_cmp(&ns[1], &t2) == 0, "%s", str.str);

    STP_Number_destroy(&t2);
    STP_Number_destroy(&t2);

    for (uint64_t i = 0; i < sizeof(ns) / sizeof(ns[0]); i++)
        STP_Number_destroy(&ns[i]);
    STP_String_destroy(&str);
}

TEST_LIST = {
    { "test_serialize_basic", test_serialize_basic },
    { "test_deserialize_basic", test_deserialize_basic },
    { NULL, NULL },
};
