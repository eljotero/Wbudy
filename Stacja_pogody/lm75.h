/*
    Obsługa pomiaru temperatury przy pomocy temometru LM75.

    Plik z definicjami funkcji.
*/

#ifndef __LM75_H
#define __LM75_H

#include "general.h"
#include "i2c.h"

tS8 measureTemperature(tU8 addr,
                       tU8 *pBuf,
                       tU16 len);

tS8 calculateTemperatureValue(tU8 *byteArray);

#endif