#ifndef TLS2561_H
#define TLS2561_H

#include "general.h"
#include "i2c.h"
#include "math.h"

tU64 measureBrightness(void);

tU64 calculateBrightness(tU16 channel0, tU16 channel1);

tU64 calculateBrightnessFloat(tU16 channel0, tU16 channel1);

#endif