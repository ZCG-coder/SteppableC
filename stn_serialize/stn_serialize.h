#pragma once

#include "stp_number.h"

int STP_Number_serialize(STP_Number* const nums, uint64_t count, char* const filename);

int STP_Number_deserialize(char* const filename, STP_Number* nums, uint64_t count);
