/*
    Obsługa pomiaru temperatury przy pomocy temometru LM75.

    Plik z definicjami funkcji.
*/

#ifndef LM75_H
#define LM75_H

#include "general.h"
#include "i2c.h"
#include "additional.h"
#include "lcd.h"

tS8 measureTemperature(tU8 addr, tU8 *pBuf);

void calculateTemperatureValue(tU8 *byteArray);

#endif
