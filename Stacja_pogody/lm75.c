/*
    Obsługa pomiaru temperatury przy pomocy temometru LM75.

    Plik z kodem źródłowym funkcji.
*/

#include "lm75.h"

void calculateTemperatureValue(tU8 *byteArray)
{
    // Variable used further for iterating in a loop
	tU8 i;

    // Char array storing string to be shown to the user
    tU8 charArray[10] = {0};

    // Return code of sprintf ; -1 if error occured
    tS8 returnValue = 0;

    // Getting temperature value represented by first 8 bits - that is its integer part
    tS8 calculatedValue = (tS8)byteArray[0];
    tU8 isNegative = 0;
    tU8 appendHalf = 0;

    // Checking if measured temperature is below zero - if yes then LSB must be handled differently.
    if (calculatedValue < 0) {
    	isNegative = 1;
    }

    if ((isNegative == (tU8)1) && ((byteArray[1] & (tU8)0x80) != (tU8)0)){
    	// Case for negative temperature with LSB set on.
        calculatedValue = (tS8)(calculatedValue + 1);
    	appendHalf = 1;
    } else if ((isNegative == (tU8)0) && ((byteArray[1] & (tU8)0x80) != (tU8)1)) {
    	// Case for positive temperature with LSB set on.
        appendHalf = 1;
    } else {
        // Case for both positive and negative temperature when LSB is not set on.
        appendHalf = 0;
    }

    // This if else statement is used for formatting string, that will be shown to the user.
    if (appendHalf == (tU8)1) {
        // Case for both positive and negative temperature with LSB set on.
    	returnValue = sprintf(charArray, "%d.5 C", calculatedValue);
    } else {
        // Case for negative temperature with LSB that is not set on.
    	returnValue = sprintf(charArray, "%d C", calculatedValue);
    }

    tU8 charPtr[1];
    if (returnValue == (tS8)(-1)) {
        charPtr[0] = strncpy(charArray, "Error", 6);
    }
    lcdPuts(charPtr[0]);
}

void measureTemperature(void)
{
    tS8 returnValue = 0;
    // LM75 thermometer address, when soldered:
    tU8 lm75Address = 0x48;     // 0x48 = 0b1001000
    // LM75 thermometer address, when not soldered:
    // tU8 lm75Address = 0x9F;  // 0x9F = 0b1001111

    // Address of LM75 used for reading data from LM75 thermometer.
    tU8 lm75ReadAddress = ((tU8)(lm75Address << 1) | (tU8)1);

    // tU8 array to hold 2 bytes containing read temperature.
    tU8 readTemperature[2] = {0};
    
    // Reading temperature value from LM75 thermometer.
    returnValue = i2cRead(lm75Address, readTemperature, 2);
    if (returnValue != I2C_CODE_ERROR) {
        calculateTemperatureValue(readTemperature);
    }
}
