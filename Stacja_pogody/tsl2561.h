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

tU16 measureBrightness();

tU16 calculateBrightness(tU16 channel0, tU16 channel1);

#endif
