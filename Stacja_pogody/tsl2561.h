/*
    Plik nagłówkowy, zawierający definicję funkcji przeznaczonych
    do obsługi czujnika oświetlenia TLS2561.
*/

#ifndef _TLS2561_H
#define _TLS2561_H

#include "general.h"

tU16 measureBrightness();

tU16 calculateBrightness(tU16 channel0, tU16 channel1);

#endif