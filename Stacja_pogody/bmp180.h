#ifndef BMP180_H
#define BMP180_H

#include "general.h"
#include "additional.h"

tS64 measurePressure(void);

tS64 calculatePressure(tU64 readPress, tU64 readTemp, tU16 *calibrationArray, tU8 pressureOss);

#endif