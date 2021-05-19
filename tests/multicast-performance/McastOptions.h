/*
 *
 * Author: Clemens Bönnen (clemens.boennen@rwth-aachen.de)
 */

// Options for measurement

#ifndef MCAST_OPTIONS_H
#define MCAST_OPTIONS_H

#define NUM_SAMPLES 100
#define NUM_ROUNDS 20
#define SIZE_SAMPLES 256
/* Size Packet:
 * 4 Byte: DATA header
 * 2 Byte: type
 * 4 Byte: id
 * 4 Byte: timestamp
 * 6 Byte: string header  --> 20 Byte
 *   rest: string content
 */
#define STRING_LENGTH (SIZE_SAMPLES - 20)
#define USE_INTERNAL_CLOCK 1

#endif