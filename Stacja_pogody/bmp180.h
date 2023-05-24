#ifndef BMP180_H
#define BMP180_H

#include "general.h"
#include "additional.h"
#include "i2c.h"

tS32 measurePressure(void);

tS32 calculatePressure(tU32 readPress, tU16 readTemp, tU16 *calibrationArray, tU8 pressureOss)

#endif