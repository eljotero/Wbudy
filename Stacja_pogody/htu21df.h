/*
    Plik nagłówkowy zawierający definicje funkcji
    przeznaczonych do obsługi czujnika wilgotności HTU21D(F).h 
*/

#ifndef HTU21DF_H
#define HTU21DF_H

#include "general.h"
#include "additional.h"
#include "i2c.h"

tS8 calculateHumidity(tU8 *readData);

tS8 measureHumidity(void);

#endif

