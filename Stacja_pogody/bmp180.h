#ifndef BMP180_H
#define BMP180_H

#include "general.h"
#include "additional.h"
#include "i2c.h"

tS64 measurePressure(void);

tS64 calculatePressure(tS64 readPress, tS64 readTemp, tU16 *calibrationArray, tU8 pressureOss);

#endif