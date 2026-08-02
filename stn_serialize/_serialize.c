#include "_utils.h"
#include "helpers.h"
#include "stp_number.h"

#include <stdint.h>
#include <stdio.h>
#include <string.h>

int STP_Number_serialize(STP_Number* const nums, uint64_t count, char* const filename)
{
    if (nums == NULL)
        return 0;

    if (count == 0)
        return 1;

    FILE* file = fopen(filename, "wb");
    if (!file)
    {
        fprintf(stderr, "%s: unable to open %s for writing", STP_CURRENT_FUNCTION, filename);
        return 0;
    }

    fwrite(HEADER, sizeof(char), sizeof(HEADER) / sizeof(char), file);
    fwrite(&count, sizeof(uint64_t), 1, file);

    for (uint64_t i = 0; i < count; ++i)
    {
        STP_Number number = nums[i];
        fwrite(&number.size, sizeof(uint64_t), 1, file);
        fwrite(&number.scale, sizeof(int64_t), 1, file);
        fwrite(&number.sign, sizeof(int8_t), 1, file);

        fwrite(number.arr, sizeof(uint64_t), number.size, file);
    }

    fclose(file);
    return 1;
}
