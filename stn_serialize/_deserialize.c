#include "_utils.h"
#include "helpers.h"
#include "stp_number.h"

#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int STP_Number_deserialize(char* const filename, STP_Number* nums)
{
    if (nums == NULL)
        return 0;

    FILE* file = fopen(filename, "rb");
    if (!file)
    {
        fprintf(stderr, "%s: unable to open %s for reading", STP_CURRENT_FUNCTION, filename);
        return 0;
    }

    char header[8];
    fread(header, sizeof(char), 8, file);
    if (memcmp(header, HEADER, 8) != 0)
    {
        fprintf(stderr, "%s: incorrect file header in %s", STP_CURRENT_FUNCTION, filename);
        fclose(file);
        return 0;
    }

    uint64_t count;
    fread(&count, sizeof(uint64_t), 1, file);
    for (uint64_t i = 0; i < count; i++)
    {
        STP_Number item;
        fread(&item.size, sizeof(uint64_t), 1, file);
        fread(&item.scale, sizeof(int64_t), 1, file);
        fread(&item.sign, sizeof(int8_t), 1, file);
        item.capacity = item.size + 4;

        uint64_t* mem = malloc(item.capacity * sizeof(uint64_t));
        if (!mem)
        {
            fprintf(stderr, "%s: cannot allocate memory for number", STP_CURRENT_FUNCTION);
            STP_ERRMSG(STP_CURRENT_FUNCTION, errno);
            fclose(file);
            return 0;
        }
        memset(mem, 0, item.capacity);

        fread(mem, sizeof(uint64_t), item.size, file);
        item.arr = mem;

        nums[i] = item;
    }

    fclose(file);
    return 1;
}
