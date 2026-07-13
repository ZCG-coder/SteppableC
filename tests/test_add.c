#include "_utils.h"

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main()
{
    srand(time(NULL));

    char* res;
    uint64_t std_size = 151;

    for (int i = 0; i < 20; ++i)
    {
        res = _generate_random_number(std_size);
        printf("%2d: %s\n", i, res);
        free(res);
    }

    return 0;
}
