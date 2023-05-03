/*
    Obsługa pomiaru temperatury przy pomocy temometru LM75.

    Plik z kodem źródłowym funkcji.
*/

#include "lm75.h"

tS8 measureTemperature(tU8 addr, tU8 *pBuf)
{
    return i2cRead(addr, pBuf, 2);
}

void calculateTemperatureValue(tU8 *byteArray)
{
    // Variable for holding result of sprintf
    tU8 *resultPtr;

    // Char array used for storing temperature to be shown to the user.
    tU8 charArray[10] = {0};

    // Variable for string integer part of the temperature
    // Since the temperature can be negative (when the 8-th bit is up then this variable must be tS8).
    tS8 calculatedValue = byteArray[0];

    // Flag indicating whether temperature value is below zero or not.
    tU8 isNegative = 0;

    // Flag indicating whether it is required to add 0.5 Celsius degree to measured temperature.
    tU8 appendHalf = 0;

    if (calculatedValue < 0) {
    	// Case for negative temperature - if temperature is in fact below zero, then adding 0.5 degree will work different way.
        isNegative = 1;
    }
    if ((isNegative == (tU8)1) && ((byteArray[1] & (tU8)0x80) == (tU8)1)){
        // Case for temperature that is negative and most significant bit in LSB (that is for 0.5 degree bit) is up.    	
        calculatedValue = calculatedValue + (tS8)1;
    	appendHalf = 1;
    } else if ((isNegative == (tU8)0) && ((byteArray[1] & (tU8)0x80) == (tU8)1)) {
        // Case for temperature that is non negative and most significant bit in LSB (that is for 0.5 degree bit) is up. 
    	appendHalf = 1;
    } else {
        ;
    }

    // Variable used as iterator in a loop.
	tU8 i = 0;

    // Parsing calculatedValue into charArray to change it accroding to appendHalf flag value
    resultPtr = sprintf(charArray, "%d", calculatedValue);

    while(charArray[i] != (tU8)0) {
        i = i + (tU8)1;
    }

    charArray[i] = '.';
    if (appendHalf == (tU8)1) {
        charArray[i + (tU8)1] = '5';
    } else {
        charArray[i + (tU8)1] = '0';
    }

    lcdPuts(charArray);
}
