/*
    Plik nagłówkowy zawierający definicje funkcji
    przeznaczonych do obsługi czujnika wilgotności HTU21D(F).h 
*/

#ifndef _HTU21DF_H
#define _HTU21DF_H

#include "general.h"

tU16 measureTemperature(tU8 address,
                        tU8 *pBuf,
                        tU16 length);

tS16 calculateHumidity(tU16 readData);

#endif