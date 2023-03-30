/*
    Plik nagłówkowy dla funkcji obsługujących
    czujnik wilgotności HTU21D(F).
*/

#ifndef __HTU21DF_h
#define __HTU21DF_h

#include "general.h"

tU8 measureHumidity(tU8 addr,
                    tU8 *pBuf,
                    tU16 len);

tU8 calculateHumidityValue(tU8 *byteArray);

#endif