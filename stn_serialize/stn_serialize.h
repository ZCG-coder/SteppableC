#pragma once

#include "stp_number.h"

/**
 * Serialize an array of count STP_Number objects (nums) to filename.
 *
 * ARG nums
 * ARG count
 * ARG filename
 * RETURNS 1 if successful, 0 otherwise
 * REQUIRES nums to contain more or equal to count STP_Number objects
 *
 * NOTE it is recommended to end filename with ".stn"
 */
int STP_Number_serialize(STP_Number* const nums, uint64_t count, char* const filename);

/**
 * Deserialize STP_Number objects from a filename.
 *
 * ARG filename
 * ARG nums
 * ARG count
 * RETURNS 1 if successful, 0 otherwise
 * REQUIRES filename is an existent file
 * REQUIRES filename contains at least count STP_Number objects
 */
int STP_Number_deserialize(char* const filename, STP_Number* nums, uint64_t count);
