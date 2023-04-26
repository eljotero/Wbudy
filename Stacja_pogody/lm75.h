/*
    Obsługa pomiaru temperatury przy pomocy temometru LM75.
    Plik z definicjami funkcji.
*/

#ifndef LM75_H
#define LM75_H

#include "general.h"
#include "additional.h"
#include "i2c.h"
#include "lcd.h"

void calculateTemperatureValue(tU8 *byteArray);

void measureTemperature(void);

#endif
