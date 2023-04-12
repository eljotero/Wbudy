/*
    Plik nagłówkowy zawierający definicje funkcji przeznaczonych do obsługi 
    czujnika ciśnienia BMP180.
*/

#ifndef _BMP180_H
#define _BMP180_H

#include "general.h"

tU16 measurePressure();

tU16 calculatePressure(tU16 readPress, tU16 readTemp, tU16 *compenstationData, tU8 pressureOss);

#endif