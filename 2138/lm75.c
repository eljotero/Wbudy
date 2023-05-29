/******************************************************************************
 *  Obsługa pomiaru temperatury przy pomocy temometru LM75.
 * 
 *  Plik z kodem źródłowym funkcji.
 *****************************************************************************/

/******************************************************************************
 * Includes
 *****************************************************************************/

#include "lm75.h"

/******************************************************************************
 * Defines and typedefs
 *****************************************************************************/

#define lm75Address         0x48 // 0x48 = 0b 0100 1000
#define lm75ReadAddress     0X91 // 0X91 = 0b 1001 0001
#define lm75WriteAddress    0X90 // 0X90 = 0b 1001 0000
#define lm75AddressSoldered         0x4F // 0x4F = 0b 0100 1111
#define lm75ReadAddressSoldered     0x9F // 0x9F = 0b 1001 1111
#define lm75WriteAddressSoldered    0x9E // 0x9E = 0b 1001 1110

/*****************************************************************************
 * Global variables
 ****************************************************************************/

/*****************************************************************************
 * Local variables
 ****************************************************************************/

/*****************************************************************************
 * Local prototypes
 ****************************************************************************/

/*
 * @brief   Funkcja measureTemperature() jest wykorzystywana do odczytania z urządzenia LM75 nieprzeliczonej
 *          temperatury, oraz jej przeliczenie na prawidłową, aktualnie występującą temperaturę, w skali
 *          Celsjusza.
 *
 * @param   void
 *
 * @returns void
 *
 * @side effects:
 *          Wartość temperatury podana jest z dokładnością do 0.5 stopnia Celsjusza, co stanowi rozdzielczość
 *          termometru LM75 (jednostkową, minimalną różnicę pomiędzy temperaturami).
 */

void measureTemperature(void)
{
    // Variable holding result of I2C operation.
    tU8 retCode = 0;

    // Buffer for read temperature values
    tU8 readTemperature[2] = {0};

    // Reading temperature value from LM75 device
    retCode = i2cRead(lm75ReadAddress, readTemperature, 2);

    // Char array used for storing temperature to be shown to the user.
    tU8 charArray[10] = {0};

    // Variable for string integer part of the temperature
    // Since the temperature can be negative (when the 8-th bit is up then this variable must be tS8).
    tS8 calculatedValue = readTemperature[0];

    // Flag indicating whether temperature value is below zero or not.
    tU8 isNegative = 0;

    // Flag indicating whether it is required to add 0.5 Celsius degree to measured temperature.
    tU8 appendHalf = 0;

    if (calculatedValue < 0) {
    	// Case for negative temperature - if temperature is in fact below zero, then adding 0.5 degree will work different way.
        isNegative = 1;
    }
    if ((isNegative == (tU8)1) && ((readTemperature[1] & (tU8)0x80) == (tU8)0x80)){
        // Case for temperature that is negative and most significant bit in LSB (that is for 0.5 degree bit) is up.    	
        calculatedValue = calculatedValue + (tS8)1;
    	appendHalf = 1;
    } else if ((isNegative == (tU8)0) && ((readTemperature[1] & (tU8)0x80) == (tU8)0x80)) {
        // Case for temperature that is non negative and most significant bit in LSB (that is for 0.5 degree bit) is up. 
    	appendHalf = 1;
    } else {
        ;
    }

    // Variable used as iterator in a loop.
	tU8 i = 0;

    // Parsing calculatedValue into charArray to change it accroding to appendHalf flag value
    retCode = sprintf(charArray, "%d", calculatedValue);

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
