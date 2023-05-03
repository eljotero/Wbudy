/*
    Plik nagłówkowy zawierający definicje funkcji
    przeznaczonych do obsługi czujnika wilgotności HTU21D(F).h 
*/

#ifndef HTU21DF_H
#define HTU21DF_H

#include "general.h"
#include "i2c.h"
#include "additional.h"

tS8 measureHumidity(tU8 address, tU8 *pBuf);

tS8 calculateHumidity(tU8 *readData);

#endif

