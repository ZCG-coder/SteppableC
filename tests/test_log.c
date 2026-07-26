#include "../stp_number/_utils.h"
#include "stp_number.h"

#include <stdio.h>

int main(void)
{
    STP_Number n;
    STP_Number_conv(&n, "1.0");

    STP_Number_ln(&n, 50);
    _STP_PRINT_NUM(n, "n");

    STP_Number_destroy(&n);
    return 0;
}
