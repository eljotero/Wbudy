#ifndef _BMP180_H
#define _BMP180_H

#include "general.h"
#include "additional.h"

tS64 measurePressure();

tS64 calculatePressure(tS16 readPress, tS32 readTemp, tU16 *calibrationArray, tU8 pressureOss);

#endif